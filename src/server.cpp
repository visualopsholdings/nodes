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
#include <cctype>

#define HEARTBEAT_INTERVAL 5 // in seconds

namespace nodes {

// handlers
void loginMsg(Server *server, json &json);
void policyUsersMsg(Server *server, json &json);
void policyGroupsMsg(Server *server, json &json);
void certsMsg(Server *server, json &json);
void usersMsg(Server *server, json &json);
void userMsg(Server *server, json &json);
void objectsMsg(Server *server, json &json);
void objectMsg(Server *server, json &json);
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
void addObjectMsg(Server *server, json &json);
void deleteObjectMsg(Server *server, json &json);
void moveObjectMsg(Server *server, json &json);
void addGroupMsg(Server *server, json &json);
void setGroupMsg(Server *server, json &json);
void setObjectMsg(Server *server, json &json);
void deleteGroupMsg(Server *server, json &json);
void setObjectPolicyMsg(Server *server, json &json);
void setGroupPolicyMsg(Server *server, json &json);
void shareLinkMsg(Server *server, json &json);
void canRegisterMsg(Server *server, json &json);
void deleteUserMsg(Server *server, json &json);
void nodesMsg(Server *server, json &json);
void nodeMsg(Server *server, json &json);
void addNodeMsg(Server *server, json &json);
void deleteNodeMsg(Server *server, json &json);
void purgeCountGroupsMsg(Server *server, json &json);
void purgeGroupsMsg(Server *server, json &json);
void purgeCountMsg(Server *server, json &json);
void purgeMsg(Server *server, json &json);
void purgeCountUsersMsg(Server *server, json &json);
void purgeUsersMsg(Server *server, json &json);

// remoteDataReq handlers
void upstreamMsg(Server *server, json &json);
void dateMsg(Server *server, json &json);
void sendOnMsg(Server *server, json &json);
void discoverLocalResultMsg(Server *server, json &json);
void discoverResultMsg(Server *server, json &json);
void ackMsg(Server *server, json &json);

// remoteMsgSub handlers
void updSubMsg(Server *server, json &json);
void addSubMsg(Server *server, json &json);

// dataRep handles
void onlineMsg(Server *server, json &json);
void discoverLocalMsg(Server *server, json &json);
void heartbeatMsg(Server *server, json &json);
void discoverMsg(Server *server, json &json);
void queryDrMsg(Server *server, json &json);
void updDrMsg(Server *server, json &json);
void addDrMsg(Server *server, json &json);

}

Server::Server(bool test, bool noupstream, 
    int pub, int rep, int dataRep, int msgPub, int remoteDataReq, int remoteMsgSub, 
    const string &dbConn, const string &dbName, const string &schema, 
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
  _messages["users"] = bind(&nodes::usersMsg, this, placeholders::_1);
  _messages["user"] = bind(&nodes::userMsg, this, placeholders::_1);
  _messages["objects"] = bind(&nodes::objectsMsg, this, placeholders::_1);
  _messages["object"] = bind(&nodes::objectMsg, this, placeholders::_1);
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
  _messages["addobject"] = bind(&nodes::addObjectMsg, this, placeholders::_1);
  _messages["deleteobject"] = bind(&nodes::deleteObjectMsg, this, placeholders::_1);
  _messages["moveobject"] = bind(&nodes::moveObjectMsg, this, placeholders::_1);
  _messages["addgroup"] = bind(&nodes::addGroupMsg, this, placeholders::_1);
  _messages["setgroup"] = bind(&nodes::setGroupMsg, this, placeholders::_1);
  _messages["setobject"] = bind(&nodes::setObjectMsg, this, placeholders::_1);
  _messages["deletegroup"] = bind(&nodes::deleteGroupMsg, this, placeholders::_1);
  _messages["setobjectpolicy"] = bind(&nodes::setObjectPolicyMsg, this, placeholders::_1);
  _messages["setgrouppolicy"] = bind(&nodes::setGroupPolicyMsg, this, placeholders::_1);
  _messages["sharelink"] = bind(&nodes::shareLinkMsg, this, placeholders::_1);
  _messages["canreg"] = bind(&nodes::canRegisterMsg, this, placeholders::_1);
  _messages["deleteuser"] = bind(&nodes::deleteUserMsg, this, placeholders::_1);
  _messages["nodes"] = bind(&nodes::nodesMsg, this, placeholders::_1);
  _messages["node"] = bind(&nodes::nodeMsg, this, placeholders::_1);
  _messages["addnode"] = bind(&nodes::addNodeMsg, this, placeholders::_1);
  _messages["deletenode"] = bind(&nodes::deleteNodeMsg, this, placeholders::_1);
  _messages["purgecountgroups"] = bind(&nodes::purgeCountGroupsMsg, this, placeholders::_1);
  _messages["purgegroups"] = bind(&nodes::purgeGroupsMsg, this, placeholders::_1);
  _messages["purgecount"] = bind(&nodes::purgeCountMsg, this, placeholders::_1);
  _messages["purge"] = bind(&nodes::purgeMsg, this, placeholders::_1);
  _messages["purgecountusers"] = bind(&nodes::purgeCountUsersMsg, this, placeholders::_1);
  _messages["purgeusers"] = bind(&nodes::purgeUsersMsg, this, placeholders::_1);

  _remoteDataReqMessages["upstream"] =  bind(&nodes::upstreamMsg, this, placeholders::_1);
  _remoteDataReqMessages["date"] =  bind(&nodes::dateMsg, this, placeholders::_1);
  _remoteDataReqMessages["queryResult"] =  bind(&nodes::sendOnMsg, this, placeholders::_1);
  _remoteDataReqMessages["discoverLocalResult"] =  bind(&nodes::discoverLocalResultMsg, this, placeholders::_1);
  _remoteDataReqMessages["discoverResult"] =  bind(&nodes::discoverResultMsg, this, placeholders::_1);
  _remoteDataReqMessages["ack"] =  bind(&nodes::ackMsg, this, placeholders::_1);

  _remoteMsgSubMessages["upd"] =  bind(&nodes::updSubMsg, this, placeholders::_1);
  _remoteMsgSubMessages["mov"] =  bind(&nodes::updSubMsg, this, placeholders::_1); // same handler as upd
  _remoteMsgSubMessages["add"] =  bind(&nodes::addSubMsg, this, placeholders::_1);

  _dataRepMessages["online"] =  bind(&nodes::onlineMsg, this, placeholders::_1);
  _dataRepMessages["discoverLocal"] =  bind(&nodes::discoverLocalMsg, this, placeholders::_1);
  _dataRepMessages["heartbeat"] =  bind(&nodes::heartbeatMsg, this, placeholders::_1);
  _dataRepMessages["discover"] =  bind(&nodes::discoverMsg, this, placeholders::_1);
  _dataRepMessages["query"] =  bind(&nodes::queryDrMsg, this, placeholders::_1);
  _dataRepMessages["upd"] =  bind(&nodes::updDrMsg, this, placeholders::_1);
  _dataRepMessages["mov"] =  bind(&nodes::updDrMsg, this, placeholders::_1); // same handler as upd
  _dataRepMessages["add"] =  bind(&nodes::addDrMsg, this, placeholders::_1);
  
  Storage::instance()->init(dbConn, dbName, schema);
  
}
  
Server::~Server() {
}
  
void Server::runUpstreamOnly() {

  BOOST_LOG_TRIVIAL(trace) << "running with upstream only";
  BOOST_LOG_TRIVIAL(info) << "init nodes";
  zmq::pollitem_t items [] = {
      { *_rep, 0, ZMQ_POLLIN, 0 },
      { _remoteDataReq->socket(), 0, ZMQ_POLLIN, 0 },
      { _remoteMsgSub->socket(), 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (!_reload) {
  
    // check connection events for upstream stuff.
    _remoteDataReq->check();
    if (_online) {
      sendUpHeartbeat();
    }
    else {
//      BOOST_LOG_TRIVIAL(trace) << "not online yet";
    }
    _remoteMsgSub->check();

//    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
    zmq::message_t message;
    zmq::poll(&items[0], 3, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      if (!getMsg("<-", *_rep, _messages)) {
        sendErr("error in getting rep message");
      }
    }
    if (items[1].revents & ZMQ_POLLIN) {
      getMsg("<-rdr", _remoteDataReq->socket(), _remoteDataReqMessages);
    }
    if (items[2].revents & ZMQ_POLLIN) {
      if (!getMsg("<-ms", _remoteMsgSub->socket(), _remoteMsgSubMessages)) {
        sendErr("error in getting upstream rep message");
      }
    }
  }
}

void Server::runUpstreamDownstream() {

  BOOST_LOG_TRIVIAL(trace) << "running with upstream and downstream";
  BOOST_LOG_TRIVIAL(info) << "init nodes";
  zmq::pollitem_t items [] = {
      { *_rep, 0, ZMQ_POLLIN, 0 },
      { _remoteDataReq->socket(), 0, ZMQ_POLLIN, 0 },
      { _dataRep->socket(), 0, ZMQ_POLLIN, 0 },
      { _remoteMsgSub->socket(), 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (!_reload) {
  
    // check connection events for upstream stuff.
    _remoteDataReq->check();
    if (_online) {
      sendUpHeartbeat();
    }
    else {
//      BOOST_LOG_TRIVIAL(trace) << "not online yet";
    }
    _remoteMsgSub->check();

//    BOOST_LOG_TRIVIAL(debug) << "polling for messages";
    zmq::message_t message;
    zmq::poll(&items[0], 4, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      if (!getMsg("<-", *_rep, _messages)) {
        sendErr("error in getting rep message");
      }
    }
    if (items[1].revents & ZMQ_POLLIN) {
      getMsg("<-rdr", _remoteDataReq->socket(), _remoteDataReqMessages);
    }
    if (items[2].revents & ZMQ_POLLIN) {
      if (!getMsg("<-dr", _dataRep->socket(), _dataRepMessages)) {
        sendErr("error in getting upstream rep message");
      }
    }
    if (items[3].revents & ZMQ_POLLIN) {
      if (!getMsg("<-ms", _remoteMsgSub->socket(), _remoteMsgSubMessages)) {
        sendErr("error in getting remote upstream sub message");
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
      if (!getMsg("<-dr", _dataRep->socket(), _dataRepMessages)) {
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
      BOOST_LOG_TRIVIAL(error) << "unknown msg type " << type.value() << " for " << name;
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

void Server::setSrc(boost::json::object *m) {

  if (!Json::appendArray(m, "path", _serverId)) {
    BOOST_LOG_TRIVIAL(error) << _serverId << " is already in the path!";
  }
  
}

bool Server::getSrc(json &msg, string *s) {

  return Json::arrayTail(msg.as_object(), "path", s);

}

bool Server::wasFromUs(json &msg) {

  // make sure it's not on the path either.
  return Json::arrayHas(msg.as_object(), "path", _serverId);
  
}

void Server::sendDown(const json &m) {

  sendTo(_dataRep->socket(), m, "dr-> ", nullopt);
  
}

void Server::pubDown(const json &m) {

  // no destination for publishes.
  boost::json::object m2 = m.as_object();
  m2.erase("dest");
  
  sendTo(_msgPub->socket(), m2, "mp-> ", nullopt);

}

void Server::sendOn(const json &origm) {

  auto data = Json::getObject(origm, "data");
  if (!data) {
    BOOST_LOG_TRIVIAL(error) << "sendOn message has no data";
    return;
  }
  auto type = Json::getString(data.value(), "type");
  if (!type) {
    BOOST_LOG_TRIVIAL(error) << "sendOn message data has no type";
    return;
  }
  auto obj = Json::getObject(data.value(), "obj");
  if (!obj) {
    BOOST_LOG_TRIVIAL(error) << "sendOn message data has no obj";
    return;
  }
  
  boost::json::object msg = origm.as_object();
  setSrc(&msg);

  if (hasUpstream() && (isObjUpstream(obj.value().as_object()) || isObjParentUpstream(type.value(), obj.value().as_object()))) {
    BOOST_LOG_TRIVIAL(trace) << "sendOn";
    msg["dest"] = _upstreamId;
    sendDataReq(nullopt, msg);
  }
  
  // try to find the id. It's either in the object or in the message data
  auto id = Json::getString(data.value(), "id", true);
  if (!id) {
    id = Json::getString(obj.value(), "id");
  }
  if (!id) {
    BOOST_LOG_TRIVIAL(error) << "sendOn message data and obj has no id";
    return;
  }
  
  if (hasValidNodes() && shouldSendDown("on", type.value(), id.value(), obj.value().as_object())) {
    BOOST_LOG_TRIVIAL(trace) << "publish down";
    pubDown(msg);
  }
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

void Server::sendAck(optional<string> result) {

  boost::json::object msg = { 
    { "type", "ack" }
  };
  if (result) {
    msg["result"] = result.value();
  }
  send(msg);
  
}

void Server::sendAckDown(optional<string> result) {

  boost::json::object msg = { 
    { "type", "ack" }
  };
  if (result) {
    msg["result"] = result.value();
  }
  sendDown(msg);
  
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

  sendTo(_remoteDataReq->socket(), m, "rdr-> ", corr);
  
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
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)  
  _remoteMsgSub->socket().setsockopt(ZMQ_SUBSCRIBE, 0, 0);
#else
  _remoteMsgSub->socket().set(zmq::sockopt::subscribe, "");
#endif
    
}

void Server::sendUpOnline() {

  BOOST_LOG_TRIVIAL(trace) << "online";

  auto doc = Site().find({{}}, {}).value();
  if (!doc) {
    BOOST_LOG_TRIVIAL(info) << "no site.";
    return;
  }
  
  auto infos = Info().find({{ "type", { { "$in", {"hasInitialSync", "upstreamMirror"}}} }}, {"type", "text"}).values();
  string hasInitialSync = Info::getInfoSafe(infos, "hasInitialSync", "false");
  string upstreamMirror = Info::getInfoSafe(infos, "upstreamMirror", "false");

  boost::json::object msg = {
    { "type", "online" },
    { "build", "28474" },
    { "headerTitle", doc.value().headerTitle() },
    { "streamBgColor", doc.value().streamBgColor() },
    { "pubKey", _pubKey },
    { "synced", hasInitialSync == "true" },
    { "mirror", upstreamMirror == "true" }
  };
  setSrc(&msg);  
  sendDataReq(nullopt, msg);
  
}

void Server::sendUpHeartbeat() {

  // only every now and then.
  time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  if (_lastHeartbeat > 0 && (now - _lastHeartbeat) < HEARTBEAT_INTERVAL) {
    return;
  }
  _lastHeartbeat = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  
  boost::json::object msg = {
    { "type", "heartbeat" }
  };
  setSrc(&msg);  
	sendDataReq(nullopt, msg);
	
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

string Server::getLastDate(optional<boost::json::array> rows, const string &hasInitialSync, const string &upstreamLastSeen) {

  string zd = Date::toISODate(0);
  
  if (rows) {
    // does any of the rows have a null date?
    bool hasNullDate = find_if(rows.value().begin(), rows.value().end(), [zd](auto e) { 
      return Json::getString(e, "modifyDate") == zd;
    }) != rows.value().end();
    return (hasInitialSync == "true") ? (hasNullDate ? zd : upstreamLastSeen) : zd;
  }
  
  // no rows
  return (hasInitialSync == "true") ? upstreamLastSeen : zd;
  
}

string Server::getCollName(const string &type, optional<string> coll) {

  return coll ? coll.value() : (type + "s");
  
}

void Server::sendUpDiscover() {

  auto infos = Info().find({{ "type", { { "$in", {"hasInitialSync", "upstreamLastSeen", "upstreamMirror"}}} }}, {"type", "text"}).values();
  string hasInitialSync = Info::getInfoSafe(infos, "hasInitialSync", "false");
  string upstreamLastSeen = Info::getInfoSafe(infos, "upstreamLastSeen", "");
  string upstreamMirror = Info::getInfoSafe(infos, "upstreamMirror", "");
  
  boost::json::object msg = {
    { "type", "discover" },
    { "hasInitialSync", hasInitialSync == "true" }
  };

  for (auto o: Storage::instance()->_schema) {
    auto type = Json::getString(o, "type");
    if (!type) {
      BOOST_LOG_TRIVIAL(error) << "type missing in schema obj " << o;
      return;
    }
    
    string collname = getCollName(type.value(), Json::getString(o, "coll", true));
    string lastname = type.value();
    lastname[0] = toupper(lastname[0]);
    lastname = "last" + lastname;

    vector<string> ids;
    if (upstreamMirror == "true") {
      // "*" means ALL objects.
      ids.push_back("*");
      msg[lastname] = getLastDate(nullopt, hasInitialSync, upstreamLastSeen);
    }
    else {
      auto objs = SchemaImpl::findGeneral(collname, json{{ "upstream", true }}, { "_id", "modifyDate" })->values();
      if (objs) {
        transform(objs.value().begin(), objs.value().end(), back_inserter(ids), [](auto e) {
          return e.as_object().at("id").as_string().c_str(); 
        });
      }
      msg[lastname] = getLastDate(objs, hasInitialSync, upstreamLastSeen);
    }
    msg[collname] =  boost::json::value_from(ids);
  }
  setSrc(&msg);
  
	sendDataReq(nullopt, msg);

}

void Server::collectObjs(const string &type, const string &collname, bsoncxx::document::view_or_value q, boost::json::array *data, vector<string> *policies) {

  auto result = SchemaImpl::findGeneral(collname, q, {});
  if (result) {
    auto vals = result->values();
    if (vals) {
      json obj = {
        { "type", type },
        { "objs", vals.value() }
      };
      data->push_back(obj);
      
      // collect all the policies of the objects
      for (auto i: vals.value()) {
        auto p = Json::getString(i, "policy", true);
        if (p) {
          if (find(policies->begin(), policies->end(), p.value()) == policies->end()) {
            policies->push_back(p.value());
          }
        }
      }
    }
  }

}

void Server::collectPolicies(const vector<string> &policies, boost::json::array *data) {

  auto q = SchemaImpl::idRangeQuery(policies);
  auto result = SchemaImpl::findGeneral("policies", q, {});
  if (result) {
    auto vals = result->values();
    if (vals) {
      json obj = {
        { "type", "policy" },
        { "objs", vals.value() }
      };
      data->push_back(obj);
    }
  }

}

void Server::sendUpDiscoverLocalUpstream(const string &upstreamLastSeen, optional<string> corr) {
    
  auto q = SchemaImpl::boolFieldEqualAfterDateQuery("upstream", true, upstreamLastSeen);

  boost::json::array data;
  vector<string> policies;
  for (auto o: Storage::instance()->_schema) {
  
    auto nosync = Json::getBool(o, "nosync", true);
    if (nosync && nosync.value()) {
      continue;
    }
    
    auto type = Json::getString(o, "type");
    if (!type) {
      BOOST_LOG_TRIVIAL(error) << "type missing in schema obj " << o;
      return;
    }
    
    string collname = getCollName(type.value(), Json::getString(o, "coll", true));
    
    collectObjs(type.value(), collname, q, &data, &policies);
    
    auto subobj = Json::getObject(o, "subobj", true);
    if (subobj) {
      auto field = Json::getString(subobj.value(), "field");
      if (!field) {
        BOOST_LOG_TRIVIAL(error) << "field missing in schema subobj " << subobj.value();
        return;
      }
      auto result = SchemaImpl::findGeneral(collname, json{ { "upstream", true } }, { "_id" });
      if (result) {
        auto upstreams = result->values();
        if (upstreams) {
          auto subtype = Json::getString(subobj.value(), "type");
          if (!subtype) {
            BOOST_LOG_TRIVIAL(error) << "type missing in schema subobj " << subobj.value();
            return;
          }
          string subcollname = getCollName(subtype.value(), Json::getString(subobj.value(), "coll", true));
          for (auto o: upstreams.value()) {
            auto id = Json::getString(o, "id");
            collectObjs(subtype.value(), subcollname, SchemaImpl::stringFieldEqualAfterDateQuery(field.value(), id.value(), upstreamLastSeen), &data, &policies);
          }
        }
      }
    }
  }
  
  // we don't send policies upstream.
  // that's why the policies array is ignored.
  boost::json::object msg = {
    { "type", "discoverLocal" },
    { "data", data }
  };
  setSrc(&msg);  
  sendDataReq(corr, msg);
  
}

void Server::sendUpDiscoverLocalMirror(const string &upstreamLastSeen, optional<string> corr) {
    
  auto q = SchemaImpl::afterDateQuery(upstreamLastSeen);

  boost::json::array data;
  vector<string> policies;
  for (auto o: Storage::instance()->_schema) {
  
    auto nosync = Json::getBool(o, "nosync", true);
    if (nosync && nosync.value()) {
      continue;
    }
    
    auto type = Json::getString(o, "type");
    if (!type) {
      BOOST_LOG_TRIVIAL(error) << "type missing in schema obj " << o;
      return;
    }
    
    string collname = getCollName(type.value(), Json::getString(o, "coll", true));
    
    collectObjs(type.value(), collname, q, &data, &policies);
    
    auto subobj = Json::getObject(o, "subobj", true);
    if (subobj) {
      auto field = Json::getString(subobj.value(), "field");
      if (!field) {
        BOOST_LOG_TRIVIAL(error) << "field missing in schema subobj " << subobj.value();
        return;
      }
      auto result = SchemaImpl::findGeneral(collname, json{{}}, { "_id" });
      if (result) {
        auto upstreams = result->values();
        if (upstreams) {
          auto subtype = Json::getString(subobj.value(), "type");
          if (!subtype) {
            BOOST_LOG_TRIVIAL(error) << "type missing in schema subobj " << subobj.value();
            return;
          }
          string subcollname = getCollName(subtype.value(), Json::getString(subobj.value(), "coll", true));
          for (auto o: upstreams.value()) {
            auto id = Json::getString(o, "id");
            collectObjs(subtype.value(), subcollname, SchemaImpl::afterDateQuery(upstreamLastSeen), &data, &policies);
          }
        }
      }
    }
  }
  
  collectPolicies(policies, &data);

  boost::json::object msg = {
    { "type", "discoverLocal" },
    { "data", data }
  };
  setSrc(&msg);  
  sendDataReq(corr, msg);

}

void Server::sendUpDiscoverLocal(optional<string> corr) {

  auto infos = Info().find({{ "type", { { "$in", {"hasInitialSync", "upstreamLastSeen", "upstreamMirror"}}} }}, {"type", "text"}).values();
  string hasInitialSync = Info::getInfoSafe(infos, "hasInitialSync", "false");
  string upstreamLastSeen = Info::getInfoSafe(infos, "upstreamLastSeen", "");
  string upstreamMirror = Info::getInfoSafe(infos, "upstreamMirror", "");
  
  if (hasInitialSync != "true") {
    BOOST_LOG_TRIVIAL(trace) << "no initial sync, nothing to discover locally.";
    boost::json::array empty;
    boost::json::object msg = {
      { "type", "discoverLocal" },
      { "data", empty }
    };
    setSrc(&msg);  
    sendDataReq(corr, msg);
    return;
  }
  if (upstreamLastSeen == "") {
    BOOST_LOG_TRIVIAL(error) << "initial sync, but no upstreamLastSeen?";
    return;
  }

  // use the schema to 
  BOOST_LOG_TRIVIAL(trace) << "upstream since " << upstreamLastSeen;
  
  if (upstreamMirror == "true") {
    sendUpDiscoverLocalMirror(upstreamLastSeen, corr);
  }
  else {
    sendUpDiscoverLocalUpstream(upstreamLastSeen, corr);
  }
}

void Server::sendDownDiscoverResult(json &j) {

  string src;
  if (!getSrc(j, &src)) {
    sendErrDown("discover missing src");
    return;
  }

  auto node = Node().find(json{ { "serverId", src } }, {}).value();
  if (!node) {
    sendErrDown("discover no node");
    return;
  }
  
  if (!node.value().valid()) {
    sendErrDown("discover node invalid");
    return;
  }

  boost::json::array msgs;
  boost::json::object obj;
  vector<string> policies;
  for (auto o: Storage::instance()->_schema) {
  
    auto nosync = Json::getBool(o, "nosync", true);
    if (nosync && nosync.value()) {
      continue;
    }
    
    auto type = Json::getString(o, "type");
    if (!type) {
      BOOST_LOG_TRIVIAL(error) << "type missing in schema obj " << o;
      return;
    }
    
    string collname = getCollName(type.value(), Json::getString(o, "coll", true));

    string lastname = type.value();
    lastname[0] = toupper(lastname[0]);
    lastname = "last" + lastname;
    BOOST_LOG_TRIVIAL(trace) << lastname;
    
    auto objsname = collname;
    BOOST_LOG_TRIVIAL(trace) << objsname;

    auto last = Json::getString(j, lastname, true);
    auto objs = Json::getArray(j, objsname, true);
    if (last) {
      obj[lastname] = last.value();
    }
    if (objs) {
      obj[objsname] = objs.value();
    }
    
    if (last && objs && objs.value().size() > 0) {
      collectObjs(type.value(), collname, SchemaImpl::idRangeAfterDateQuery(objs.value(), last.value()), &msgs, &policies);
      
      auto subobj = Json::getObject(o, "subobj", true);
      if (subobj) {
        auto field = Json::getString(subobj.value(), "field");
        if (!field) {
          BOOST_LOG_TRIVIAL(error) << "field missing in schema subobj " << subobj.value();
          return;
        }
        auto subtype = Json::getString(subobj.value(), "type");
        if (!subtype) {
          BOOST_LOG_TRIVIAL(error) << "type missing in schema subobj " << subobj.value();
          return;
        }
        string subcollname = getCollName(subtype.value(), Json::getString(subobj.value(), "coll", true));
        if (objs.value().size() == 1 && objs.value()[0] == "*") {
          // collect ALL sub objects after the date.
          auto objs = SchemaImpl::findGeneral(collname, json{{}}, { "_id" })->values();
          for (auto o: objs.value()) {
            collectObjs(subtype.value(), subcollname, 
              SchemaImpl::stringFieldEqualAfterDateQuery(field.value(), Json::getString(o, "id").value(), last.value()), &msgs, &policies);
          }
        }
        else {
          for (auto s: objs.value()) {
            collectObjs(subtype.value(), subcollname, 
              SchemaImpl::stringFieldEqualAfterDateQuery(field.value(), s.as_string().c_str(), last.value()), &msgs, &policies);
          }
        }
      }
    }
  
  }

  collectPolicies(policies, &msgs);
  
  // update the node where we are.  
  Node().updateById(node.value().id(), obj);

  // and send the result on.
  sendDown({
    { "type", "discoverResult" },
    { "msgs", msgs },
    
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
    auto objs = Json::getArray(m, "objs");
    if (!objs) {
      BOOST_LOG_TRIVIAL(error) << "msg missing objs";
      continue;
    }
    
    string coll;
    if (!Storage::instance()->collName(type.value(), &coll, false)) {
      continue;
    }

    Storage::instance()->bulkInsert(coll, objs.value());

    auto more = Json::getBool(m, "more", true);
    if (more && more.value()) {
      BOOST_LOG_TRIVIAL(info) << "ignoring more for " << type.value();
      continue;
    }
  }

}

bool Server::isValidId(const string &id) {
  return id.size() == 24;
}

bool Server::validateId(boost::json::object &obj, const string &id) {

  auto objid = Json::getString(obj, "id", true);
  if (!objid) {
    return true;
  }
  if (objid.value() != id) {
    BOOST_LOG_TRIVIAL(trace) << "skipping update, ids don't match " << objid.value() << " " << id;
    return false;
  }
  return true;
}

vector<string> Server::getNodeIds(const string &type) {

  auto nodes = Node().find(json{{ "valid", true }}).values();
  if (!nodes || nodes.value().size() == 0) {
    return {};
  }
  string field = type + "s";
  vector<string> ids;
  for (auto n: nodes.value()) {
    auto nj = n.j();
    auto a = Json::getArray(nj, field);
    if (a) {
      for (auto i: a.value()) {
        string id(i.as_string());
        if (find(ids.begin(), ids.end(), id) == ids.end()) {
          ids.push_back(id);
        }
      }
    }
  }
  BOOST_LOG_TRIVIAL(trace) << "getNodeIds " << boost::algorithm::join(ids, ", ");
  
  return ids;

}

bool Server::hasValidNodes() {

  BOOST_LOG_TRIVIAL(trace) << "hasValidNodes";

  if (_dataRep) {
    auto nodes = Node().find(json{ { "valid", true }}).values();
    if (!nodes || nodes.value().size() == 0) {
      BOOST_LOG_TRIVIAL(trace) << "skipping send down mov nobody listening at all";
      return false;
    }
    return true;
  }
  
  return false;
  
}

bool Server::anyNodesListening(const string &type, const string &id) {

  BOOST_LOG_TRIVIAL(trace) << "anyNodesListening";

  // search for it.
  auto ids = getNodeIds(type);
  if (find(ids.begin(), ids.end(), "*") != ids.end() || find(ids.begin(), ids.end(), id) != ids.end()) {
    return true;
  }
  BOOST_LOG_TRIVIAL(trace) << "skipping mov nobody listening " << type << " " << id;
  return false;

}

bool Server::shouldSendDown(const string &action, const string &type, const string &id, boost::json::object &obj) {

  BOOST_LOG_TRIVIAL(trace) << "shouldSendDown";

  if (action == "add") {
    // always send adds, people can't be listening to them.
    // non mirrors will get them, it's up to them to ignore them.
    return true;
  }
  
  // calculate the type of object and the id of the object to search for.
  string ltype;
  string iid;
  string pfield;
  if (Storage::instance()->parentInfo(type, &pfield, &ltype)) {
    auto iparent = Json::getString(obj, pfield);
    if (!iparent) {
      BOOST_LOG_TRIVIAL(error) << "shouldSendDown obj has no parent field";
      return false;
    }
    iid = iparent.value();
  }
  else {
    ltype = type;
    iid = id;
  }
  
  return anyNodesListening(ltype, iid);
  
}

bool Server::isObjParentUpstream(const string &type, boost::json::object &obj) {

  BOOST_LOG_TRIVIAL(trace) << "isObjParentUpstream";

  string ptype;
  string pfield;
  if (Storage::instance()->parentInfo(type, &pfield, &ptype)) {
    auto iparent = Json::getString(obj, pfield);
    if (!iparent) {
      BOOST_LOG_TRIVIAL(error) << "obj is missing " << pfield;
      return false;
    }
    string coll;
    if (!Storage::instance()->collName(ptype, &coll)) {
      return false;
    }

    auto result = SchemaImpl::findByIdGeneral(coll, iparent.value(), { "upstream" });
    if (!result) {
      BOOST_LOG_TRIVIAL(error) << "Can't find " << iparent.value();
      return false;
    }
    
    auto s = result->value();
    if (s) {
      auto upstream = Json::getBool(s.value(), "upstream", true);
      return upstream && upstream.value();
    }
  }
  return false;

}

bool Server::isParentUpstream(const string &ptype, const string &origparent) {

  BOOST_LOG_TRIVIAL(trace) << "isParentUpstream";

  string coll;
  if (!Storage::instance()->collName(ptype, &coll)) {
    return false;
  }
  
  auto result = SchemaImpl::findByIdGeneral(coll, origparent, { "upstream" });
  if (!result) {
    BOOST_LOG_TRIVIAL(error) << "Can't find " << origparent;
    return false;
  }
  auto s = result->value();
  if (s) {
    auto upstream = Json::getBool(s.value(), "upstream", true);
    return upstream && upstream.value();
  }

  BOOST_LOG_TRIVIAL(trace) << "not sending up";
  
  return false;
}

bool Server::hasUpstream() {

  BOOST_LOG_TRIVIAL(trace) << "hasUpstream " << _upstreamId;

  return _upstreamId != "";
  
}

bool Server::isObjUpstream(boost::json::object &obj) {

  BOOST_LOG_TRIVIAL(trace) << "isObjUpstream";

  string mirror = get1Info("upstreamMirror");
  if (mirror == "true") {
    BOOST_LOG_TRIVIAL(trace) << "mirror sending up";
    return true;
  }
  auto upstream = Json::getBool(obj, "upstream", true);
  if (upstream && upstream.value()) {
    BOOST_LOG_TRIVIAL(trace) << "upstream sending up";
    return true;
  }

  return false;
  
}

void Server::sendUpd(const string &type, const string &id, boost::json::object &obj) {

  BOOST_LOG_TRIVIAL(trace) << "sendUpd " << type;

  if (!isValidId(id)) {
    BOOST_LOG_TRIVIAL(warning) << "skipping upd, obj id is not valid" << id;
    return;
  }
  if (!validateId(obj, id)) {
    return;
  }
  
  bool up = false;
  if (hasUpstream()) {
    up = isObjUpstream(obj);
    if (!up) {
      up = isObjParentUpstream(type, obj);
    }
  }
  
  bool down = false;
  if (hasValidNodes()) {
    down = shouldSendDown("update", type, id, obj);
  }
  
  if (!up && !down) {
    BOOST_LOG_TRIVIAL(trace) << "not sending up or down";
    return;
  }
  
  if (obj.empty()) {
    return;
  }

  boost::json::object msg = {
    { "type", "upd" },
    { "data", {
      { "type", type },
      { "id", id },
      { "obj", obj }
      }
    }
  };
  setSrc(&msg);
  if (down) {
    pubDown(msg);
  }
  msg["dest"] = _upstreamId;
  if (up) {
    sendDataReq(nullopt, msg);
  }

}

void Server::sendAdd(const string &type, boost::json::object &obj) {

  BOOST_LOG_TRIVIAL(trace) << "sendAdd " << type;

  auto id = Json::getString(obj, "id");
  if (!id) {
    BOOST_LOG_TRIVIAL(warning) << "skipping add, but obj id ";
  }
    
  bool up = false;
  if (hasUpstream()) {
    up = isObjUpstream(obj);
    if (!up) {
      up = isObjParentUpstream(type, obj);
    }
  }
  bool down = false;
  if (hasValidNodes()) {
    down = shouldSendDown("add", type, id.value(), obj);
  }
  
  if (!up && !down) {
    BOOST_LOG_TRIVIAL(trace) << "not sending";
    return;
  }
  
  if (obj.empty()) {
    return;
  }

  boost::json::object msg = {
    { "type", "add" },
    { "data", {
      { "type", type },
      { "obj", obj }
      }
    }
  };
  
  setSrc(&msg);
  if (down) {
    pubDown(msg);
  }
  msg["dest"] = _upstreamId;
  if (up) {
    sendDataReq(nullopt, msg);
  }

}

void Server::sendMov(const string &type, const string &id, boost::json::object &obj, const string &ptype, const string &origparent) {

  BOOST_LOG_TRIVIAL(trace) << "sendMov" << type;

  if (!isValidId(id)) {
    BOOST_LOG_TRIVIAL(warning) << "skipping mov, obj id is not valid" << id;
    return;
  }
  if (!validateId(obj, id)) {
    return;
  }
  
  bool up = false;
  if (hasUpstream()) {
    up = isObjUpstream(obj);
    if (!up) {
      up = isObjParentUpstream(type, obj);
    }
    if (!up) {
      up = isParentUpstream(ptype, origparent);
    }
  }
  bool down = false;
  if (hasValidNodes()) {
    down = anyNodesListening(ptype, origparent);
    if (!down) {
      down = shouldSendDown("move", type, id, obj);
    }
  }
  
  if (!up && !down) {
    BOOST_LOG_TRIVIAL(trace) << "not sending up or down";
    return;
  }
  
  if (obj.empty()) {
    return;
  }

  boost::json::object msg = {
    { "type", "mov" },
    { "data", {
      { "type", type },
      { "id", id },
      { "obj", obj },
      { "orig", origparent }
      }
    }
  };
  setSrc(&msg);
  if (down) {
    pubDown(msg);
  }
  msg["dest"] = _upstreamId;
  if (up) {
    sendDataReq(nullopt, msg);
  }

}

bool Server::updateObject(json &j) {

  auto data = Json::getObject(j, "data");
  if (!data) {
    BOOST_LOG_TRIVIAL(error) << "upd sub missing data";
    return false;
  }
  auto type = Json::getString(data.value(), "type");
  if (!type) {
    BOOST_LOG_TRIVIAL(error) << "upd sub data missing type";
    return false;
  }
  auto id = Json::getString(data.value(), "id");
  if (!id) {
    BOOST_LOG_TRIVIAL(error) << "upd sub data missing id";
    return false;
  }
  auto obj = Json::getObject(data.value(), "obj");
  if (!obj) {
    BOOST_LOG_TRIVIAL(error) << "upd sub data missing obj";
    return false;
  }
  
  string coll;
  if (!Storage::instance()->collName(type.value(), &coll, false)) {
    return false;
  }

  auto result = SchemaImpl::updateGeneralById(coll, id.value(), {{ "$set", obj.value() }});
  if (!result) {
    BOOST_LOG_TRIVIAL(error) << "upd sub failed to update db";
    return false;
  }

  return true;
  
}

bool Server::addObject(json &j) {

  auto data = Json::getObject(j, "data");
  if (!data) {
    BOOST_LOG_TRIVIAL(error) << "add sub missing data";
    return false;
  }
  auto type = Json::getString(data.value(), "type");
  if (!type) {
    BOOST_LOG_TRIVIAL(error) << "add sub data missing type";
    return false;
  }
  auto obj = Json::getObject(data.value(), "obj");
  if (!obj) {
    BOOST_LOG_TRIVIAL(error) << "add sub data missing obj";
    return false;
  }
  
  // clean up the object.
  boost::json::object obj2 = obj.value().as_object();
  obj2.erase("id");
  obj2.erase("type");
  
  string coll;
  if (!Storage::instance()->collName(type.value(), &coll, false)) {
    return false;
  }

  auto result = SchemaImpl::insertGeneral(coll, obj2);
  if (!result) {
    BOOST_LOG_TRIVIAL(error) << "upd sub failed to update db";
    return false;
  }

  return true;
  
}

bool Server::shouldIgnoreAdd(json &msg) {

  string mirror = get1Info("upstreamMirror");
  if (mirror == "true") {
    // always add when we are a mirror.
    return false;
  }
  
  auto data = Json::getObject(msg, "data");
  if (!data) {
    return true;
  }
  
  auto type = Json::getString(data.value(), "type");
  if (!type) {
    return true;
  }
  
  auto obj = Json::getObject(data.value(), "obj");
  if (!obj) {
    return true;
  }

  if (!isObjParentUpstream(type.value(), obj.value().as_object())) {
    BOOST_LOG_TRIVIAL(trace) << type.value() << " parent upstream, ignoring add";
    return true;
  }

  return false;
  
}