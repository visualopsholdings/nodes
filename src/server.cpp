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
#include "encrypter.hpp"

#include <boost/log/trivial.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp> 
#include <boost/algorithm/string.hpp>

#define HEARTBEAT_INTERVAL 5 // in seconds

namespace nodes {

// handlers
void loginMsg(Server *server, json &json);
void policyUsersMsg(Server *server, json &json);
void policyGroupsMsg(Server *server, json &json);
void messageMsg(Server *server, json &json);
void certsMsg(Server *server, json &json);
void usersMsg(Server *server, json &json);
void userMsg(Server *server, json &json);
void streamsMsg(Server *server, json &json);
void streamMsg(Server *server, json &json);
void ideasMsg(Server *server, json &json);
void infosMsg(Server *server, json &json);
void setinfoMsg(Server *server, json &json);
void siteMsg(Server *server, json &json);
void setsiteMsg(Server *server, json &json);
void queryMsg(Server *server, json &json);
void newUserMsg(Server *server, json &json);
void reloadMsg(Server *server, json &json);
void groupsMsg(Server *server, json &json);
void groupMsg(Server *server, json &json);
void membersMsg(Server *server, json &json);
void searchUsersMsg(Server *server, json &json);
void newMemberMsg(Server *server, json &json);
void deleteMemberMsg(Server *server, json &json);
void setuserMsg(Server *server, json &json);
void policyMsg(Server *server, json &json);
void addStreamMsg(Server *server, json &json);
void deleteStreamMsg(Server *server, json &json);
void addGroupMsg(Server *server, json &json);
void setGroupMsg(Server *server, json &json);
void setStreamMsg(Server *server, json &json);
void deleteGroupMsg(Server *server, json &json);
void setStreamPolicyMsg(Server *server, json &json);
void setGroupPolicyMsg(Server *server, json &json);
void streamShareLinkMsg(Server *server, json &json);
void canRegisterMsg(Server *server, json &json);

// dataReq handlers
void upstreamMsg(Server *server, json &json);
void dateMsg(Server *server, json &json);
void sendOnMsg(Server *server, json &json);
void discoverLocalResultMsg(Server *server, json &json);
void discoverResultMsg(Server *server, json &json);

}

Server::Server(bool test, bool noupstream, int pub, int rep, int dataReq, int msgSub, 
      const string &dbConn, const string &dbName, const string &certFile, const string &chainFile, const string &hostName) :
    _test(test), _certFile(certFile), _chainFile(chainFile), _dataReqPort(dataReq), _msgSubPort(msgSub),
    _online(false), _lastHeartbeat(0), _noupstream(noupstream), _reload(false), _hostName(hostName) {

  _context.reset(new zmq::context_t(1));
  _pub.reset(new zmq::socket_t(*_context, ZMQ_PUB));
  _pub->bind("tcp://127.0.0.1:" + to_string(pub));
	BOOST_LOG_TRIVIAL(info) << "Bound to ZMQ as Local PUB on " << pub;

  _rep.reset(new zmq::socket_t(*_context, ZMQ_REP));
  _rep->bind("tcp://127.0.0.1:" + to_string(rep));
	BOOST_LOG_TRIVIAL(info) << "Bound to ZMQ as Local REP on " << rep;

  _messages["certs"] = bind(&nodes::certsMsg, this, placeholders::_1);
  _messages["login"] = bind(&nodes::loginMsg, this, placeholders::_1);
  _messages["policyusers"] = bind(&nodes::policyUsersMsg, this, placeholders::_1);
  _messages["policygroups"] = bind(&nodes::policyGroupsMsg, this, placeholders::_1);
  _messages["message"] = bind(&nodes::messageMsg, this, placeholders::_1);
  _messages["users"] = bind(&nodes::usersMsg, this, placeholders::_1);
  _messages["user"] = bind(&nodes::userMsg, this, placeholders::_1);
  _messages["streams"] = bind(&nodes::streamsMsg, this, placeholders::_1);
  _messages["stream"] = bind(&nodes::streamMsg, this, placeholders::_1);
  _messages["ideas"] = bind(&nodes::ideasMsg, this, placeholders::_1);
  _messages["infos"] = bind(&nodes::infosMsg, this, placeholders::_1);
  _messages["setinfo"] = bind(&nodes::setinfoMsg, this, placeholders::_1);
  _messages["site"] = bind(&nodes::siteMsg, this, placeholders::_1);
  _messages["setsite"] = bind(&nodes::setsiteMsg, this, placeholders::_1);
  _messages["query"] = bind(&nodes::queryMsg, this, placeholders::_1);
  _messages["newuser"] = bind(&nodes::newUserMsg, this, placeholders::_1);
  _messages["reload"] = bind(&nodes::reloadMsg, this, placeholders::_1);
  _messages["groups"] = bind(&nodes::groupsMsg, this, placeholders::_1);
  _messages["group"] = bind(&nodes::groupMsg, this, placeholders::_1);
  _messages["members"] = bind(&nodes::membersMsg, this, placeholders::_1);
  _messages["searchusers"] = bind(&nodes::searchUsersMsg, this, placeholders::_1);
  _messages["newmember"] = bind(&nodes::newMemberMsg, this, placeholders::_1);
  _messages["deletemember"] = bind(&nodes::deleteMemberMsg, this, placeholders::_1);
  _messages["setuser"] = bind(&nodes::setuserMsg, this, placeholders::_1);
  _messages["policy"] = bind(&nodes::policyMsg, this, placeholders::_1);
  _messages["addstream"] = bind(&nodes::addStreamMsg, this, placeholders::_1);
  _messages["deletestream"] = bind(&nodes::deleteStreamMsg, this, placeholders::_1);
  _messages["addgroup"] = bind(&nodes::addGroupMsg, this, placeholders::_1);
  _messages["setgroup"] = bind(&nodes::setGroupMsg, this, placeholders::_1);
  _messages["setstream"] = bind(&nodes::setStreamMsg, this, placeholders::_1);
  _messages["deletegroup"] = bind(&nodes::deleteGroupMsg, this, placeholders::_1);
  _messages["setstreampolicy"] = bind(&nodes::setStreamPolicyMsg, this, placeholders::_1);
  _messages["setgrouppolicy"] = bind(&nodes::setGroupPolicyMsg, this, placeholders::_1);
  _messages["streamsharelink"] = bind(&nodes::streamShareLinkMsg, this, placeholders::_1);
  _messages["canreg"] = bind(&nodes::canRegisterMsg, this, placeholders::_1);

  _dataReqMessages["upstream"] =  bind(&nodes::upstreamMsg, this, placeholders::_1);
  _dataReqMessages["date"] =  bind(&nodes::dateMsg, this, placeholders::_1);
  _dataReqMessages["queryResult"] =  bind(&nodes::sendOnMsg, this, placeholders::_1);
  _dataReqMessages["discoverLocalResult"] =  bind(&nodes::discoverLocalResultMsg, this, placeholders::_1);
  _dataReqMessages["discoverResult"] =  bind(&nodes::discoverResultMsg, this, placeholders::_1);
  
  Storage::instance()->init(dbConn, dbName);
  
}
  
Server::~Server() {
}
  
void Server::run() {

  while (1) {
    if (_dataReq) {
      BOOST_LOG_TRIVIAL(trace) << "running with upstream";
      zmq::pollitem_t items [] = {
          { *_rep, 0, ZMQ_POLLIN, 0 },
          { _dataReq->socket(), 0, ZMQ_POLLIN, 0 }
      };
      const std::chrono::milliseconds timeout{500};
      while (!_reload) {
      
        // check connection events for upstream stuff.
        _dataReq->check();
        if (_online) {
          heartbeat();
        }
        else {
          BOOST_LOG_TRIVIAL(trace) << "not online yet";
        }
        if (_msgSub) {
          _msgSub->check();
        }
    
    //    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
        zmq::message_t message;
        zmq::poll(&items[0], 2, timeout);
      
        if (items[0].revents & ZMQ_POLLIN) {
          if (!getMsg("rep", *_rep, _messages)) {
            sendErr("error in getting rep message");
          }
        }
        if (items[1].revents & ZMQ_POLLIN) {
          getMsg("dataReq", _dataReq->socket(), _dataReqMessages);
        }
      }
    }
    else {
      BOOST_LOG_TRIVIAL(trace) << "running standalone";
      zmq::pollitem_t items [] = {
          { *_rep, 0, ZMQ_POLLIN, 0 }
      };
      const std::chrono::milliseconds timeout{500};
      while (!_reload) {
      
    //    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
        zmq::message_t message;
        zmq::poll(&items[0], 1, timeout);
      
        if (items[0].revents & ZMQ_POLLIN) {
          if (!getMsg("rep", *_rep, _messages)) {
            sendErr("error in getting rep message");
          }
        }
      }
    }
    _reload = false;
  }

}

bool Server::getMsg(const string &name, zmq::socket_t &socket, map<string, msgHandler> &handlers ) {

  BOOST_LOG_TRIVIAL(trace) << "got " << name << " message";
  zmq::message_t reply;
  try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
    socket.recv(&reply);
#else
    auto res = socket.recv(reply, zmq::recv_flags::none);
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

void Server::sendTo(zmq::socket_t &socket, const json &j, const string &type) {

  stringstream ss;
  ss << j;
  string m = ss.str();
  
  BOOST_LOG_TRIVIAL(debug) << type << " " << m;

	zmq::message_t msg(m.length());
	memcpy(msg.data(), m.c_str(), m.length());
  try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
    socket.send(msg);
#else
    socket.send(msg, zmq::send_flags::none);
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
    { "level", "fatal" }, 
    { "msg", msg } 
  });
  
}

void Server::sendWarning(const string &msg) {

  BOOST_LOG_TRIVIAL(error) << msg;
  send({ 
    { "type", "err" }, 
    { "level", "warning" }, 
    { "msg", msg } 
  });
  
}

void Server::sendAck() {

  send({ 
    { "type", "ack" } 
  });
  
}

void Server::sendDataReq(const json &m) {

  sendTo(_dataReq->socket(), m, "dataReq");
  
}

void Server::stopUpstream() {

  if (_dataReq) {
    _dataReq->socket().close();
    _dataReq.reset();
  }
  if (_msgSub) {
    _msgSub->socket().close();
    _msgSub.reset();
  }

}

void Server::clearUpstream() {

  Info().deleteMany({{ "type", { { "$in", {"upstream", "upstreamPubKey", "hasInitialSync", "upstreamLastSeen"}}} }});

  systemStatus("Server orphaned");
  stopUpstream();

}

void Server::connectUpstream() {

  int major, minor, patch;
  zmq_version(&major, &minor, &patch);
  BOOST_LOG_TRIVIAL(info) << "zmq version " << major << "." << minor << "." << patch;
  
  if (!zmq_has("curve")) {
    BOOST_LOG_TRIVIAL(info) << "no curve available";
    return;
  }

  stopUpstream();
  
  auto docs = Info().find({{ "type", { { "$in", {"serverId", "upstream", "upstreamPubKey", "privateKey", "pubKey"}}} }}, {"type", "text"}).values();
  if (!docs) {
    BOOST_LOG_TRIVIAL(info) << "no infos.";
    return;
  }
  auto serverId = Info::getInfo(docs.value(), "serverId");
  if (!serverId) {
    BOOST_LOG_TRIVIAL(info) << "no serverId.";
    return;
  }
  auto upstream = Info::getInfo(docs.value(), "upstream");
  if (!upstream) {
    BOOST_LOG_TRIVIAL(info) << "no upstream.";
    return;
  }
  auto upstreamPubKey = Info::getInfo(docs.value(), "upstreamPubKey");
  if (!upstreamPubKey) {
    BOOST_LOG_TRIVIAL(error) << "upstream, but no upstreamPubKey";
    return;
  }
  auto privateKey = Info::getInfo(docs.value(), "privateKey");
  if (!privateKey) {
    BOOST_LOG_TRIVIAL(error) << "upstream, but no privateKey";
    return;
  }
  auto pubKey = Info::getInfo(docs.value(), "pubKey");
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
  
  sendDataReq({
    { "type", "online" },
    { "build", "28474" },
    { "headerTitle", doc.value().headerTitle() },
    { "streamBgColor", doc.value().streamBgColor() },
    { "hasInitialSync", "true" },
    { "pubKey", _pubKey },
    { "src", _serverId },
//    { "dest", _upstreamId }    
  });
  
}

void Server::heartbeat() {

  // only every now and then.
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  if (_lastHeartbeat > 0 && (now - _lastHeartbeat) < HEARTBEAT_INTERVAL) {
    return;
  }
  _lastHeartbeat = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  
	sendDataReq({
    { "type", "heartbeat" },
    { "src", _serverId }
  });
	
}

bool Server::setInfo(const string &name, const string &text) {

  auto doc = Info().find({{ "type", name }}, {"text"}).value();
  if (doc) {
    BOOST_LOG_TRIVIAL(trace) << "info old value " << doc.value().j();
    auto result = Info().update({{ "type", name }}, {{ "text", text }});
    if (!result) {
      BOOST_LOG_TRIVIAL(error) << "could not update info";
      return false;
    }
    BOOST_LOG_TRIVIAL(trace) << "updated " << result.value();
    return true;
  }

  auto result = Info().insert({
    { "type", name },
    { "text", text }
  });
  if (!result) {
    BOOST_LOG_TRIVIAL(error) << "could not insert info";
    return false;
  }
  BOOST_LOG_TRIVIAL(trace) << "inserted " << result.value();
  return true;
  
}

bool Server::resetServer() {
  
  char pubkey[41];
  char seckey[41];
  int rc = zmq_curve_keypair(pubkey, seckey);
  if (rc != 0) {
    BOOST_LOG_TRIVIAL(error) << "failed to generate key pair " << rc;
    return false;
  }
  
  if (!setInfo("privateKey", seckey)) {
    return false;
  }
  if (!setInfo("pubKey", pubkey)) {
    return false;
  }
  if (!setInfo("tokenKey", Encrypter::makeKey())) {
    return false;
  }
  if (!setInfo("tokenIV", Encrypter::makeIV())) {
    return false;
  }
  // generate a new server ID.
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  stringstream ss;
  ss << uuid;
  if (!setInfo("serverId", ss.str())) {
    return false;
  }
    
  // clear out all these flags.
  Info().deleteMany({{ "type", { { "$in", {"upstream", "upstreamPubKey", "upstreamMirror", 
    "hasInitialSync", "upstreamLastSeen" }}} }});
  
  return true;
}

string Server::get1Info(const string &type) {

  auto docs = Info().find({{ "type", type }}).values();
  if (!docs) {
    return "";
  }
  auto info = Info::getInfo(docs.value(), type);
  if (!info) {
    return "";
  }
  return info.value();

}

void Server::systemStatus(const string &msg) {

  publish({
    { "type", "status" },
    { "text", msg }
  });
  
}

void Server::discover() {

  auto hasInitialSync = get1Info("hasInitialSync");
  auto upstreamLastSeen = get1Info("upstreamLastSeen");
  auto users = User().find(json{{ "upstream", true }}, {{ "_id" }}).values();
  if (!users) {
    BOOST_LOG_TRIVIAL(error) << "no users";
    return;
  }
  vector<string> ids;
  transform(users.value().begin(), users.value().end(), back_inserter(ids), [](auto e){ return e.id(); });
  
  json zd = {{ "$date", 0 }};
	sendDataReq({
    { "type", "discover" },
    { "lastUser", hasInitialSync == "true" ? upstreamLastSeen : Json::toISODate(zd) },
    { "users", boost::json::value_from(ids) },
    { "hasInitialSync", hasInitialSync == "true" },
    { "src", _serverId }
  });

}
