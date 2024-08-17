/*
  server.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "storage/schema.hpp"
#include "upstream.hpp"

#include <boost/log/trivial.hpp>

#define HEARTBEAT_INTERVAL 5 // in seconds

Server::Server(bool test, bool noupstream, int pub, int rep, int dataReq, int msgSub, 
      const string &dbConn, const string &dbName, const string &certFile, const string &chainFile) :
    _test(test), _certFile(certFile), _chainFile(chainFile), _dataReqPort(dataReq), _msgSubPort(msgSub),
    _online(false), _lastHeartbeat(0), _noupstream(noupstream) {

  _context.reset(new zmq::context_t(1));
  _pub.reset(new zmq::socket_t(*_context, ZMQ_PUB));
  _pub->bind("tcp://127.0.0.1:" + to_string(pub));
	BOOST_LOG_TRIVIAL(info) << "Bound to ZMQ as Local PUB on " << pub;

  _rep.reset(new zmq::socket_t(*_context, ZMQ_REP));
  _rep->bind("tcp://127.0.0.1:" + to_string(rep));
	BOOST_LOG_TRIVIAL(info) << "Bound to ZMQ as Local REP on " << rep;

  _messages["certs"] = bind(&Server::certsMsg, this, placeholders::_1);
  _messages["login"] = bind(&Server::loginMsg, this, placeholders::_1);
  _messages["policyusers"] = bind(&Server::policyUsersMsg, this, placeholders::_1);
  _messages["message"] = bind(&Server::messageMsg, this, placeholders::_1);
  _messages["users"] = bind(&Server::usersMsg, this, placeholders::_1);
  _messages["user"] = bind(&Server::userMsg, this, placeholders::_1);
  _messages["streams"] = bind(&Server::streamsMsg, this, placeholders::_1);
  _messages["stream"] = bind(&Server::streamMsg, this, placeholders::_1);
  _messages["ideas"] = bind(&Server::ideasMsg, this, placeholders::_1);
  _messages["infos"] = bind(&Server::infosMsg, this, placeholders::_1);
  _messages["setinfo"] = bind(&Server::setinfoMsg, this, placeholders::_1);
  _messages["site"] = bind(&Server::siteMsg, this, placeholders::_1);
  _messages["setsite"] = bind(&Server::setsiteMsg, this, placeholders::_1);
  _messages["query"] = bind(&Server::queryMsg, this, placeholders::_1);

  _dataReqMessages["upstream"] =  bind(&Server::upstreamMsg, this, placeholders::_1);
  _dataReqMessages["date"] =  bind(&Server::dateMsg, this, placeholders::_1);
  _dataReqMessages["queryResult"] =  bind(&Server::sendOnMsg, this, placeholders::_1);
  
  Storage::instance()->init(dbConn, dbName);
  
}
  
Server::~Server() {
}
  
void Server::run() {

  if (_noupstream) {
    BOOST_LOG_TRIVIAL(info) << "ignoring upstream.";
  }
  else {
    connectUpstream();
  }
  
  if (_dataReq) {
    zmq::pollitem_t items [] = {
        { *_rep, 0, ZMQ_POLLIN, 0 },
        { *_dataReq, 0, ZMQ_POLLIN, 0 }
    };
    const std::chrono::milliseconds timeout{500};
    while (1) {
    
      // check connection events for upstream stuff.
      _dataReq->check();
      if (_online) {
        heartbeat();
      }
      if (_msgSub) {
        _msgSub->check();
      }
  
  //    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
      zmq::message_t message;
      zmq::poll(&items[0], 2, timeout);
    
      if (items[0].revents & ZMQ_POLLIN) {
        if (!getMsg("rep", _rep, _messages)) {
          sendErr("error in getting rep message");
        }
      }
      if (items[1].revents & ZMQ_POLLIN) {
        getMsg("dataReq", _dataReq, _dataReqMessages);
      }
    }
  }
  else {
    zmq::pollitem_t items [] = {
        { *_rep, 0, ZMQ_POLLIN, 0 }
    };
    const std::chrono::milliseconds timeout{500};
    while (1) {
    
  //    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
      zmq::message_t message;
      zmq::poll(&items[0], 1, timeout);
    
      if (items[0].revents & ZMQ_POLLIN) {
        if (!getMsg("rep", _rep, _messages)) {
          sendErr("error in getting rep message");
        }
      }
    }
  }

}

bool Server::getMsg(const string &name, shared_ptr<zmq::socket_t> socket, map<string, msgHandler> &handlers ) {

  BOOST_LOG_TRIVIAL(trace) << "got " << name << " message";
  zmq::message_t reply;
  try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
    socket->recv(&reply);
#else
    auto res = socket->recv(reply, zmq::recv_flags::none);
#endif
    // convert to JSON
    string r((const char *)reply.data(), reply.size());
    json doc = boost::json::parse(r);

    BOOST_LOG_TRIVIAL(trace) << "got " << name << " reply "<< doc;

    auto type = Json::getString(doc, "type");
    if (!type) {
      BOOST_LOG_TRIVIAL(error) << "no type for " << name << " reply";
      return false;
    }

    BOOST_LOG_TRIVIAL(debug) << "handling " << type.value();
    map<string, msgHandler>::iterator handler = handlers.find(type.value());
    if (handler == handlers.end()) {
      BOOST_LOG_TRIVIAL(error) << "unknown msg type " << type.value();
      return false;
    }
    handler->second(doc);
  }
  catch (zmq::error_t &e) {
    BOOST_LOG_TRIVIAL(warning) << "got exc with " << name << " recv" << e.what() << "(" << e.num() << ")";
  }

  return true;
  
}

void Server::sendTo(shared_ptr<zmq::socket_t> socket, const json &j, const string &type) {

  stringstream ss;
  ss << j;
  string m = ss.str();
  
  BOOST_LOG_TRIVIAL(debug) << type << " " << m;

	zmq::message_t msg(m.length());
	memcpy(msg.data(), m.c_str(), m.length());
  try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
    socket->send(msg);
#else
    socket->send(msg, zmq::send_flags::none);
#endif
  }
  catch (zmq::error_t &e) {
    BOOST_LOG_TRIVIAL(warning) << "got exc publish" << e.what() << "(" << e.num() << ")";
  }

}

json Server::receiveFrom(shared_ptr<zmq::socket_t> socket) {

  zmq::message_t reply;
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
  socket->recv(&reply);
#else
  auto res = socket->recv(reply, zmq::recv_flags::none);
#endif
  string r((const char *)reply.data(), reply.size());
  return boost::json::parse(r);

}

void Server::sendErr(const string &msg) {
  BOOST_LOG_TRIVIAL(error) << msg;
  send({ 
    { "type", "err" }, 
    { "msg", msg } 
  });
}

void Server::sendAck() {

  send({ 
    { "type", "ack" } 
  });
  
}

optional<string> Server::getInfo(const vector<InfoRow> &infos, const string &type) const {

  auto i = find_if(infos.begin(), infos.end(), 
    [&type](auto &e) { return e.type() == type; });
  if (i == infos.end()) {
    return nullopt;
  }
  return (*i).text();
}

void Server::connectUpstream() {

  if (_dataReq) {
    _dataReq->close();
    _dataReq.reset();
  }
  if (_msgSub) {
    _msgSub->close();
    _msgSub.reset();
  }
  
  auto docs = Info().find({{ "type", { { "$in", {"serverId", "upstream", "upstreamPubKey", "privateKey", "pubKey"}}} }}, {"type", "text"}).values();
  if (!docs) {
    BOOST_LOG_TRIVIAL(info) << "no infos.";
    return;
  }
  auto serverId = getInfo(docs.value(), "serverId");
  if (!serverId) {
    BOOST_LOG_TRIVIAL(info) << "no serverId.";
    return;
  }
  auto upstream = getInfo(docs.value(), "upstream");
  if (!upstream) {
    BOOST_LOG_TRIVIAL(info) << "no upstream.";
    return;
  }
  auto upstreamPubKey = getInfo(docs.value(), "upstreamPubKey");
  if (!upstreamPubKey) {
    BOOST_LOG_TRIVIAL(error) << "upstream, but no upstreamPubKey";
    return;
  }
  auto privateKey = getInfo(docs.value(), "privateKey");
  if (!privateKey) {
    BOOST_LOG_TRIVIAL(error) << "upstream, but no privateKey";
    return;
  }
  auto pubKey = getInfo(docs.value(), "pubKey");
  if (!pubKey) {
    BOOST_LOG_TRIVIAL(error) << "upstream, but no pubKey";
    return;
  }
  
  _pubKey =  pubKey.value();
  _serverId = serverId.value();
  
	BOOST_LOG_TRIVIAL(trace) << "upstream: " << upstream.value();
	BOOST_LOG_TRIVIAL(trace) << "upstreamPubKey: " << upstreamPubKey.value();
	BOOST_LOG_TRIVIAL(trace) << "privateKey: " << privateKey.value();
	BOOST_LOG_TRIVIAL(trace) << "_pubKey: " << _pubKey;

  _dataReq.reset(new Upstream(this, *_context, ZMQ_REQ, "dataReq", upstream.value(), _dataReqPort, 
    upstreamPubKey.value(), privateKey.value(), _pubKey));
  _msgSub.reset(new Upstream(this, *_context, ZMQ_SUB, "msgSub", upstream.value(), _msgSubPort, 
    upstreamPubKey.value(), privateKey.value(), _pubKey));
    
}

void Server::online() {

  BOOST_LOG_TRIVIAL(trace) << "online";

  auto doc = Site().find({{}}, {}).value();
  if (!doc) {
    BOOST_LOG_TRIVIAL(info) << "no site.";
    return;
  }
  
	sendTo(_dataReq, {
    { "type", "online" },
    { "build", "28474" },
    { "headerTitle", doc.value().headerTitle() },
    { "streamBgColor", doc.value().streamBgColor() },
    { "hasInitialSync", "true" },
    { "pubKey", _pubKey },
    { "src", _serverId },
//    { "dest", _upstreamId }    
  }, "req upstream");
  
}

void Server::heartbeat() {

  // only every now and then.
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  if (_lastHeartbeat > 0 && (now - _lastHeartbeat) < HEARTBEAT_INTERVAL) {
    return;
  }
  _lastHeartbeat = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  
	sendTo(_dataReq, {
    { "type", "heartbeat" },
    { "src", _serverId }
  }, "req heartbeat");
	
}
