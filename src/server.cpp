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
#include "downstream.hpp"
#include "encrypter.hpp"
#include "security.hpp"
#include "date.hpp"

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
void addUserMsg(Server *server, json &json);
void reloadMsg(Server *server, json &json);
void groupsMsg(Server *server, json &json);
void groupMsg(Server *server, json &json);
void membersMsg(Server *server, json &json);
void searchUsersMsg(Server *server, json &json);
void addMemberMsg(Server *server, json &json);
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
void deleteUserMsg(Server *server, json &json);
void nodesMsg(Server *server, json &json);
void nodeMsg(Server *server, json &json);
void addNodeMsg(Server *server, json &json);
void deleteNodeMsg(Server *server, json &json);

// remoteDataReq handlers
void upstreamMsg(Server *server, json &json);
void dateMsg(Server *server, json &json);
void sendOnMsg(Server *server, json &json);
void discoverLocalResultMsg(Server *server, json &json);
void discoverResultMsg(Server *server, json &json);

// dataRep handles
void onlineMsg(Server *server, json &json);
void discoverLocalMsg(Server *server, json &json);
void heartbeatMsg(Server *server, json &json);
void discoverMsg(Server *server, json &json);
void queryDrMsg(Server *server, json &json);

}

Server::Server(bool test, bool noupstream, 
    int pub, int rep, int dataRep, int msgPub, int remoteDataReq, int remoteMsgSub, 
    const string &dbConn, const string &dbName, 
    const string &certFile, const string &chainFile, 
    const string &hostName, const string &bindAddress) :
    _test(test), _certFile(certFile), _chainFile(chainFile),
    _dataRepPort(dataRep), _msgPubPort(msgPub),
    _remoteDataReqPort(remoteDataReq), _remoteMsgSubPort(remoteMsgSub),
    _online(false), _lastHeartbeat(0), _noupstream(noupstream), _reload(false), 
    _hostName(hostName), _bindAddress(bindAddress) {

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
  _messages["adduser"] = bind(&nodes::addUserMsg, this, placeholders::_1);
  _messages["reload"] = bind(&nodes::reloadMsg, this, placeholders::_1);
  _messages["groups"] = bind(&nodes::groupsMsg, this, placeholders::_1);
  _messages["group"] = bind(&nodes::groupMsg, this, placeholders::_1);
  _messages["members"] = bind(&nodes::membersMsg, this, placeholders::_1);
  _messages["searchusers"] = bind(&nodes::searchUsersMsg, this, placeholders::_1);
  _messages["addmember"] = bind(&nodes::addMemberMsg, this, placeholders::_1);
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
  _messages["deleteuser"] = bind(&nodes::deleteUserMsg, this, placeholders::_1);
  _messages["nodes"] = bind(&nodes::nodesMsg, this, placeholders::_1);
  _messages["node"] = bind(&nodes::nodeMsg, this, placeholders::_1);
  _messages["addnode"] = bind(&nodes::addNodeMsg, this, placeholders::_1);
  _messages["deletenode"] = bind(&nodes::deleteNodeMsg, this, placeholders::_1);

  _remoteDataReqMessages["upstream"] =  bind(&nodes::upstreamMsg, this, placeholders::_1);
  _remoteDataReqMessages["date"] =  bind(&nodes::dateMsg, this, placeholders::_1);
  _remoteDataReqMessages["queryResult"] =  bind(&nodes::sendOnMsg, this, placeholders::_1);
  _remoteDataReqMessages["discoverLocalResult"] =  bind(&nodes::discoverLocalResultMsg, this, placeholders::_1);
  _remoteDataReqMessages["discoverResult"] =  bind(&nodes::discoverResultMsg, this, placeholders::_1);

  _dataRepMessages["online"] =  bind(&nodes::onlineMsg, this, placeholders::_1);
  _dataRepMessages["discoverLocal"] =  bind(&nodes::discoverLocalMsg, this, placeholders::_1);
  _dataRepMessages["heartbeat"] =  bind(&nodes::heartbeatMsg, this, placeholders::_1);
  _dataRepMessages["discover"] =  bind(&nodes::discoverMsg, this, placeholders::_1);
  _dataRepMessages["query"] =  bind(&nodes::queryDrMsg, this, placeholders::_1);
  
  Storage::instance()->init(dbConn, dbName);
  
}
  
Server::~Server() {
}
  
void Server::runUpstreamOnly() {

  BOOST_LOG_TRIVIAL(trace) << "running with upstream only";
  BOOST_LOG_TRIVIAL(info) << "init nodes";
  zmq::pollitem_t items [] = {
      { *_rep, 0, ZMQ_POLLIN, 0 },
      { _remoteDataReq->socket(), 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (!_reload) {
  
    // check connection events for upstream stuff.
    _remoteDataReq->check();
    if (_online) {
      heartbeat();
    }
    else {
//      BOOST_LOG_TRIVIAL(trace) << "not online yet";
    }
    if (_remoteMsgSub) {
      _remoteMsgSub->check();
    }

//    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
    zmq::message_t message;
    zmq::poll(&items[0], 2, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      if (!getMsg("<-", *_rep, _messages)) {
        sendErr("error in getting rep message");
      }
    }
    if (items[1].revents & ZMQ_POLLIN) {
      getMsg("<-&", _remoteDataReq->socket(), _remoteDataReqMessages);
    }
  }
}

void Server::runUpstreamDownstream() {

  BOOST_LOG_TRIVIAL(trace) << "running with upstream and downstream";
  BOOST_LOG_TRIVIAL(info) << "init nodes";
  zmq::pollitem_t items [] = {
      { *_rep, 0, ZMQ_POLLIN, 0 },
      { _remoteDataReq->socket(), 0, ZMQ_POLLIN, 0 },
      { _dataRep->socket(), 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (!_reload) {
  
    // check connection events for upstream stuff.
    _remoteDataReq->check();
    if (_online) {
      heartbeat();
    }
    else {
//      BOOST_LOG_TRIVIAL(trace) << "not online yet";
    }
    if (_remoteMsgSub) {
      _remoteMsgSub->check();
    }

//    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
    zmq::message_t message;
    zmq::poll(&items[0], 3, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      if (!getMsg("<-", *_rep, _messages)) {
        sendErr("error in getting rep message");
      }
    }
    if (items[1].revents & ZMQ_POLLIN) {
      getMsg("<-&", _remoteDataReq->socket(), _remoteDataReqMessages);
    }
    if (items[2].revents & ZMQ_POLLIN) {
      if (!getMsg("<-$", _dataRep->socket(), _dataRepMessages)) {
        sendErr("error in getting upstream rep message");
      }
    }
  }
}

void Server::runStandalone() {

  BOOST_LOG_TRIVIAL(trace) << "running standalone";
  BOOST_LOG_TRIVIAL(info) << "init nodes";
  zmq::pollitem_t items [] = {
      { *_rep, 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (!_reload) {
  
//    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
    zmq::message_t message;
    zmq::poll(&items[0], 1, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      if (!getMsg("<-", *_rep, _messages)) {
        sendErr("error in getting rep message");
      }
    }
  }
  
}

void Server::runDownstreamOnly() {

  BOOST_LOG_TRIVIAL(trace) << "running downstream only";
  BOOST_LOG_TRIVIAL(info) << "init nodes";
  zmq::pollitem_t items [] = {
      { *_rep, 0, ZMQ_POLLIN, 0 },
      { _dataRep->socket(), 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (!_reload) {
  
//    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
    zmq::message_t message;
    zmq::poll(&items[0], 2, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      if (!getMsg("<-", *_rep, _messages)) {
        sendErr("error in getting rep message");
      }
    }
    if (items[1].revents & ZMQ_POLLIN) {
      if (!getMsg("<-$", _dataRep->socket(), _dataRepMessages)) {
        sendErr("error in getting upstream rep message");
      }
    }
  }
  
}

void Server::run() {

  while (1) {
    if (_remoteDataReq) {
      if (_dataRep) {
        runUpstreamDownstream();
      }
      else {
        runUpstreamOnly();
      }
    }
    else {
      if (_dataRep) {
        runDownstreamOnly();
      }
      else {
        runStandalone();
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

    BOOST_LOG_TRIVIAL(debug) << name << " " << doc;

    auto type = Json::getString(doc, "type");
    if (!type) {
      BOOST_LOG_TRIVIAL(error) << "no type for " << name << " reply";
      return false;
    }

    BOOST_LOG_TRIVIAL(trace) << "handling " << type.value();
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

void Server::sendTo(zmq::socket_t &socket, const json &j, const string &type, optional<string> corr) {

  json j2 = j;
  if (corr) {
    BOOST_LOG_TRIVIAL(trace) << "had corr id " << corr.value();
    if (type == "&->") {
      // old servers (NodeJS) still use socket id
      j2.as_object()["socketid"] = corr.value();
    }
    j2.as_object()["corr"] = corr.value();
  }

  stringstream ss;
  ss << j2;
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

void Server::sendDown(const json &m) {

  sendTo(_dataRep->socket(), m, "$-> ", nullopt);
  
}

void Server::sendErr(const string &msg) {

  BOOST_LOG_TRIVIAL(error) << msg;
  send({ 
    { "type", "err" }, 
    { "level", "fatal" }, 
    { "msg", msg } 
  });
  
}

void Server::sendErrDown(const string &msg) {

  BOOST_LOG_TRIVIAL(error) << msg;
  sendDown({ 
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

void Server::sendAckDown() {

  sendDown({ 
    { "type", "ack" } 
  });
  
}

void Server::sendSecurity() {

  BOOST_LOG_TRIVIAL(error) << "security error";
  send({ 
    { "type", "err" }, 
    { "level", "security" }
  });
  
}

bool Server::testModifyDate(json &j, const json &doc) {

  auto test = Json::getObject(j, "test", true);
  if (test) {
    auto time = Json::getString(test.value(), "time", true);
    if (time) {
      long mod = Date::fromISODate(Json::getString(doc, "modifyDate").value());
      long t = Date::fromISODate(time.value());
      if (mod <= t) {
        BOOST_LOG_TRIVIAL(trace) << "not changed " << mod << " <= " << t;
        return true;
      }
    }
  }
  return false;
}

bool Server::testCollectionChanged(json &j, const string &name) {

  auto test = Json::getObject(j, "test", true);
  if (test) {
    auto time = Json::getNumber(test.value(), "time", true);
    if (time) {
      long changed = Storage::instance()->collectionChanged(name);
      if (changed <= time.value()) {
        BOOST_LOG_TRIVIAL(trace) << "not changed " << changed << " <= " << time.value();
        return true;
      }
    }
  }
  return false;
}

void Server::sendCollection(json &j, const string &name, const boost::json::array &array) {

  if (testCollectionChanged(j, name)) {
    send({
      { "type", name },
      { "test", {
        { "latest", true }
        }
      }
    });
    return;
  }

  send({
    { "type", name },
    { "test", {
      { "time", Storage::instance()->collectionChanged(name) }
      }
    },
    { name, array }
  });

}

void Server::sendObject(json &j, const string &name, const json &doc) {

  if (testModifyDate(j, doc)) {
    send({
      { "type", name },
      { "test", {
        { "latest", true }
        }
      }
    });
    return;
  }
  
  send({
    { "type", name },
    { name, doc }
  });

}

void Server::sendDataReq(optional<string> corr, const json &m) {

  // set the src of the message.
  json m2 = m;
  m2.as_object()["src"] = _serverId;

  sendTo(_remoteDataReq->socket(), m2, "&-> ", corr);
  
}

void Server::stopUpstream() {

  if (_remoteDataReq) {
    _remoteDataReq->socket().close();
    _remoteDataReq.reset();
  }
  if (_remoteMsgSub) {
    _remoteMsgSub->socket().close();
    _remoteMsgSub.reset();
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
  
  auto privateKey = Info::getInfo(docs.value(), "privateKey");
  if (!privateKey) {
    BOOST_LOG_TRIVIAL(error) << "upstream, but no privateKey";
    return;
  }

  if (_dataRepPort && _msgPubPort) {
    _msgPub.reset(new Downstream(this, *_context, ZMQ_PUB, "msgPub", _msgPubPort, privateKey.value()));
    _dataRep.reset(new Downstream(this, *_context, ZMQ_REP, "dataRep", _dataRepPort, privateKey.value()));
  }

  auto serverId = Info::getInfo(docs.value(), "serverId");
  if (!serverId) {
    BOOST_LOG_TRIVIAL(info) << "no serverId.";
    return;
  }
  _serverId = serverId.value();

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
  auto pubKey = Info::getInfo(docs.value(), "pubKey");
  if (!pubKey) {
    BOOST_LOG_TRIVIAL(error) << "upstream, but no pubKey";
    return;
  }
  
  _pubKey =  pubKey.value();
  
	BOOST_LOG_TRIVIAL(trace) << "upstream: " << upstream.value();
	BOOST_LOG_TRIVIAL(trace) << "upstreamPubKey: " << upstreamPubKey.value();
	BOOST_LOG_TRIVIAL(trace) << "privateKey: " << privateKey.value();
	BOOST_LOG_TRIVIAL(trace) << "_pubKey: " << _pubKey;

  _remoteDataReq.reset(new Upstream(this, *_context, ZMQ_REQ, "remoteDataReq", upstream.value(), _remoteDataReqPort, 
    upstreamPubKey.value(), privateKey.value(), _pubKey));
  _remoteMsgSub.reset(new Upstream(this, *_context, ZMQ_SUB, "remoteMsgSub", upstream.value(), _remoteMsgSubPort, 
    upstreamPubKey.value(), privateKey.value(), _pubKey));
    
}

void Server::online() {

  BOOST_LOG_TRIVIAL(trace) << "online";

  auto doc = Site().find({{}}, {}).value();
  if (!doc) {
    BOOST_LOG_TRIVIAL(info) << "no site.";
    return;
  }
  
  sendDataReq(nullopt, {
    { "type", "online" },
    { "build", "28474" },
    { "headerTitle", doc.value().headerTitle() },
    { "streamBgColor", doc.value().streamBgColor() },
    { "hasInitialSync", "true" },
    { "pubKey", _pubKey },
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
  
	sendDataReq(nullopt, {
    { "type", "heartbeat" }
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
  
  // make sure to start security again.
  Security::rebuild();
  
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

  publish(nullopt, {
    { "type", "status" },
    { "text", msg }
  });
  
}

template <typename RowType>
string Server::getLastDate(optional<vector<RowType > > rows, const string &hasInitialSync, const string &upstreamLastSeen) {

  string zd = Date::toISODate(0);
  
  if (rows) {
    // does any of the rows have a null date?
    bool hasNullDate = find_if(rows.value().begin(), rows.value().end(), [zd](auto e) { 
      return Json::getString(e.j(), "modifyDate") == zd;
    }) != rows.value().end();
    return (hasInitialSync == "true") ? (hasNullDate ? zd : upstreamLastSeen) : zd;
  }
  
  // no rows
  return (hasInitialSync == "true") ? upstreamLastSeen : zd;
  
}

void Server::discover() {

  auto infos = Info().find({{ "type", { { "$in", {"hasInitialSync", "upstreamLastSeen"}}} }}, {"type", "text"}).values();
  string hasInitialSync = Info::getInfoSafe(infos, "hasInitialSync", "false");
  string upstreamLastSeen = Info::getInfoSafe(infos, "upstreamLastSeen", "");
  
  auto users = User().find(json{{ "upstream", true }}, { "_id", "modifyDate" }).values();
  
  // if we have users to discover.
  vector<string> userids;
  if (users) {
    transform(users.value().begin(), users.value().end(), back_inserter(userids), [](auto e){ return e.id(); });
  }

  auto groups = Group().find(json{{ "upstream", true }}, { "_id", "modifyDate" }).values();
  
  // if we have groups to discover.
  vector<string> groupids;
  if (groups) {
    transform(groups.value().begin(), groups.value().end(), back_inserter(groupids), [](auto e){ return e.id(); });
  }

  auto streams = Stream().find(json{{ "upstream", true }}, { "_id", "modifyDate" }).values();
  
  // if we have streams to discover.
  vector<string> streamids;
  if (streams) {
    transform(streams.value().begin(), streams.value().end(), back_inserter(streamids), [](auto e){ return e.id(); });
  }

	sendDataReq(nullopt, {
    { "type", "discover" },
    { "lastUser", getLastDate(users, hasInitialSync, upstreamLastSeen) },
    { "lastGroup", getLastDate(groups, hasInitialSync, upstreamLastSeen) },
    { "lastStream", getLastDate(streams, hasInitialSync, upstreamLastSeen) },
    { "users", boost::json::value_from(userids) },
    { "groups", boost::json::value_from(groupids) },
    { "streams", boost::json::value_from(streamids) },
    { "hasInitialSync", hasInitialSync == "true" }
  });

}

void collectObjs(const string &name, bsoncxx::document::view_or_value q, boost::json::array *data) {

  auto result = SchemaImpl::findGeneral(name + "s", q, {});
  if (result) {
    boost::json::array objs;
    auto vals = result->values();
    if (vals) {
      json obj = {
        { "type", name },
        { "objs", vals.value() }
      };
      data->push_back(obj);
    }
  }

}

void Server::discoverLocal(optional<string> corr) {

  auto infos = Info().find({{ "type", { { "$in", {"hasInitialSync", "upstreamLastSeen"}}} }}, {"type", "text"}).values();
  string hasInitialSync = Info::getInfoSafe(infos, "hasInitialSync", "false");
  string upstreamLastSeen = Info::getInfoSafe(infos, "upstreamLastSeen", "");
  
  if (hasInitialSync != "true") {
    BOOST_LOG_TRIVIAL(trace) << "no initial sync, nothing to discover locally.";
    boost::json::array empty;
    sendDataReq(corr, {
      { "type", "discoverLocal" },
      { "data", empty }
    });
    return;
  }
  if (upstreamLastSeen == "") {
    BOOST_LOG_TRIVIAL(error) << "initial sync, but no upstreamLastSeen?";
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << "upstream since " << upstreamLastSeen;
  auto q = SchemaImpl::upstreamAfterDateQuery(upstreamLastSeen);
  boost::json::array data;
  collectObjs("user", q, &data);
  collectObjs("team", q, &data);
  collectObjs("stream", q, &data);

  auto upstreams = Stream().find(json{ { "upstream", true } }, { "_id" }).values();
  if (upstreams) {
    for (auto s: upstreams.value()) {
      auto q = SchemaImpl::streamAfterDateQuery(s.id(), upstreamLastSeen);
      collectObjs("idea", q, &data);
    }
  }
  
  sendDataReq(corr, {
    { "type", "discoverLocal" },
    { "data", data }
  });

}

void Server::resetDB() {
 
  BOOST_LOG_TRIVIAL(info) << "DB reset";
  Storage::instance()->allCollectionsChanged();
   
}

void Server::importObjs(boost::json::array &msgs) {

  for (auto m: msgs) {
    auto type = Json::getString(m, "type");
    if (!type) {
      BOOST_LOG_TRIVIAL(error) << "msg missing type";
      continue;
    }
    vector<string> valid{ "user", "group", "stream", "idea" };
    if (find(valid.begin(), valid.end(), type.value()) == valid.end()) {
      BOOST_LOG_TRIVIAL(warning) << "message type " << type.value() << " not supported.";
      continue;
    }
    
    auto objs = Json::getArray(m, "objs");
    if (!objs) {
      BOOST_LOG_TRIVIAL(error) << "msg missing objs";
      continue;
    }
    
    Storage::instance()->bulkInsert(type.value() + "s", objs.value());

    auto more = Json::getBool(m, "more", true);
    if (more && more.value()) {
      BOOST_LOG_TRIVIAL(info) << "ignoring more for " << type.value();
      continue;
    }
  }

}
