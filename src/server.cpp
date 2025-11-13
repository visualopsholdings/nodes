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
#include "log.hpp"
#include "data.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp> 
#include <boost/algorithm/string.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <cctype>
#include <rfl/json.hpp>

#define HEARTBEAT_INTERVAL  5 // in seconds
#define MAX_OBJECTS         50 // maximum objects at a time.

using namespace nodes;

namespace nodes {

// handlers
void policyUsersMsg(Server *server, const IncomingMsg &in);
void policyGroupsMsg(Server *server, const IncomingMsg &in);
void usersMsg(Server *server, const IncomingMsg &in);
void userMsg(Server *server, const IncomingMsg &in);
void objectsMsg(Server *server, const IncomingMsg &in);
void objectMsg(Server *server, const IncomingMsg &in);
void infosMsg(Server *server, const IncomingMsg &in);
void siteMsg(Server *server, const IncomingMsg &in);
void groupsMsg(Server *server, const IncomingMsg &in);
void groupMsg(Server *server, const IncomingMsg &in);
void membersMsg(Server *server, const IncomingMsg &in);
void searchUsersMsg(Server *server, const IncomingMsg &in);
void setuserMsg(Server *server, const IncomingMsg &in);
void policyMsg(Server *server, const IncomingMsg &in);
void nodesMsg(Server *server, const IncomingMsg &in);
void nodeMsg(Server *server, const IncomingMsg &in);
void loginMsg(Server *server, const IncomingMsg &in);
void setinfoMsg(Server *server, const IncomingMsg &in);
void setsiteMsg(Server *server, const IncomingMsg &in);
void queryMsg(Server *server, const IncomingMsg &in);
void addUserMsg(Server *server, const IncomingMsg &in);
void reloadMsg(Server *server, const IncomingMsg &in);
void addMemberMsg(Server *server, const IncomingMsg &in);
void deleteMemberMsg(Server *server, const IncomingMsg &in);
void addObjectMsg(Server *server, const IncomingMsg &in);
void addNodeMsg(Server *server, const IncomingMsg &in);
void deleteObjectMsg(Server *server, const IncomingMsg &in);
void moveObjectMsg(Server *server, const IncomingMsg &in);
void addGroupMsg(Server *server, const IncomingMsg &in);
void setGroupMsg(Server *server, const IncomingMsg &in);
void setObjectMsg(Server *server, const IncomingMsg &in);
void deleteGroupMsg(Server *server, const IncomingMsg &in);
void setObjectPolicyMsg(Server *server, const IncomingMsg &in);
void setGroupPolicyMsg(Server *server, const IncomingMsg &in);
void deleteUserMsg(Server *server, const IncomingMsg &in);

void shareLinkMsg(Server *server, Data &data);
void canRegisterMsg(Server *server, Data &data);
void deleteNodeMsg(Server *server, Data &data);
void purgeCountGroupsMsg(Server *server, Data &data);
void purgeGroupsMsg(Server *server, Data &data);
void purgeCountMsg(Server *server, Data &data);
void purgeMsg(Server *server, Data &data);
void purgeCountUsersMsg(Server *server, Data &data);
void purgeUsersMsg(Server *server, Data &data);
void requestBuildMsg(Server *server, Data &data);
void buildMsg(Server *server, Data &data);

// remoteDataReq handlers
void discoverResultMsg(Server *server, const IncomingMsg &in);
void upstreamMsg(Server *server, const IncomingMsg &in);
void sendOnMsg(Server *server, const IncomingMsg &in);

void dateMsg(Server *server, Data &data);

// remoteMsgSub handlers
void updSubMsg(Server *server, Data &data);
void addSubMsg(Server *server, Data &data);

// dataRep handles
void discoverLocalMsg(Server *server, const IncomingMsg &in);

void onlineMsg(Server *server, Data &data);
void heartbeatMsg(Server *server, Data &data);
void queryDrMsg(Server *server, Data &data);
void updDrMsg(Server *server, Data &data);
void addDrMsg(Server *server, Data &data);

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
	L_INFO("Bound to ZMQ as Local PUB on " << pub);

  _rep.reset(new zmq::socket_t(*_context, ZMQ_REP));
  _rep->bind("tcp://127.0.0.1:" + to_string(rep));
	L_INFO("Bound to ZMQ as Local REP on " << rep);

  _messages["certs"] =  [&](Data &) {
    if (_certFile.empty()) {
      send(dictO({
        { "type", "certs" }, 
        { "ssl", false }
      }));
    }
    else {
      send(dictO({
        { "type", "certs" }, 
        { "ssl", true },
        { "certFile", _certFile }, 
        { "chainFile",_chainFile }
      }));
    }
  };
  
  _messages2["login"] = bind(&nodes::loginMsg, this, placeholders::_1);
  _messages2["policyusers"] = bind(&nodes::policyUsersMsg, this, placeholders::_1);
  _messages2["policygroups"] = bind(&nodes::policyGroupsMsg, this, placeholders::_1);
  _messages2["users"] = bind(&nodes::usersMsg, this, placeholders::_1);
  _messages2["user"] = bind(&nodes::userMsg, this, placeholders::_1);
  _messages2["objects"] = bind(&nodes::objectsMsg, this, placeholders::_1);
  _messages2["object"] = bind(&nodes::objectMsg, this, placeholders::_1);
  _messages2["infos"] = bind(&nodes::infosMsg, this, placeholders::_1);
  _messages2["setinfo"] = bind(&nodes::setinfoMsg, this, placeholders::_1);
  _messages2["site"] = bind(&nodes::siteMsg, this, placeholders::_1);
  _messages2["setsite"] = bind(&nodes::setsiteMsg, this, placeholders::_1);
  _messages2["query"] = bind(&nodes::queryMsg, this, placeholders::_1);
  _messages2["adduser"] = bind(&nodes::addUserMsg, this, placeholders::_1);
  _messages2["reload"] = bind(&nodes::reloadMsg, this, placeholders::_1);
  _messages2["groups"] = bind(&nodes::groupsMsg, this, placeholders::_1);
  _messages2["group"] = bind(&nodes::groupMsg, this, placeholders::_1);
  _messages2["members"] = bind(&nodes::membersMsg, this, placeholders::_1);
  _messages2["searchusers"] = bind(&nodes::searchUsersMsg, this, placeholders::_1);
  _messages2["setuser"] = bind(&nodes::setuserMsg, this, placeholders::_1);
  _messages2["policy"] = bind(&nodes::policyMsg, this, placeholders::_1);
  _messages2["nodes"] = bind(&nodes::nodesMsg, this, placeholders::_1);
  _messages2["node"] = bind(&nodes::nodeMsg, this, placeholders::_1);
  _messages2["addmember"] = bind(&nodes::addMemberMsg, this, placeholders::_1);
  _messages2["deletemember"] = bind(&nodes::deleteMemberMsg, this, placeholders::_1);
  _messages2["addobject"] = bind(&nodes::addObjectMsg, this, placeholders::_1);
  _messages2["addnode"] = bind(&nodes::addNodeMsg, this, placeholders::_1);
  _messages2["deleteobject"] = bind(&nodes::deleteObjectMsg, this, placeholders::_1);
  _messages2["moveobject"] = bind(&nodes::moveObjectMsg, this, placeholders::_1);
  _messages2["addgroup"] = bind(&nodes::addGroupMsg, this, placeholders::_1);
  _messages2["setgroup"] = bind(&nodes::setGroupMsg, this, placeholders::_1);
  _messages2["setobject"] = bind(&nodes::setObjectMsg, this, placeholders::_1);
  _messages2["deletegroup"] = bind(&nodes::deleteGroupMsg, this, placeholders::_1);
  _messages2["setobjectpolicy"] = bind(&nodes::setObjectPolicyMsg, this, placeholders::_1);
  _messages2["setgrouppolicy"] = bind(&nodes::setGroupPolicyMsg, this, placeholders::_1);
  _messages2["deleteuser"] = bind(&nodes::deleteUserMsg, this, placeholders::_1);

  _messages["sharelink"] = bind(&nodes::shareLinkMsg, this, placeholders::_1);
  _messages["canreg"] = bind(&nodes::canRegisterMsg, this, placeholders::_1);
  _messages["deletenode"] = bind(&nodes::deleteNodeMsg, this, placeholders::_1);
  _messages["purgecountgroups"] = bind(&nodes::purgeCountGroupsMsg, this, placeholders::_1);
  _messages["purgegroups"] = bind(&nodes::purgeGroupsMsg, this, placeholders::_1);
  _messages["purgecount"] = bind(&nodes::purgeCountMsg, this, placeholders::_1);
  _messages["purge"] = bind(&nodes::purgeMsg, this, placeholders::_1);
  _messages["purgecountusers"] = bind(&nodes::purgeCountUsersMsg, this, placeholders::_1);
  _messages["purgeusers"] = bind(&nodes::purgeUsersMsg, this, placeholders::_1);
  _messages["requestbuild"] = bind(&nodes::requestBuildMsg, this, placeholders::_1);
  _messages["build"] = bind(&nodes::buildMsg, this, placeholders::_1);

  _remoteDataReqMessages2["discoverLocalResult"] =  [&](const IncomingMsg &in) {
    // the server has inserted all the local stuff, discover what's out there.
    sendUpDiscover();
  };
  _remoteDataReqMessages2["ack"] =  [&](const IncomingMsg &in) {
    L_TRACE("ack");
  };
  _remoteDataReqMessages2["discoverResult"] =  bind(&nodes::discoverResultMsg, this, placeholders::_1);
  _remoteDataReqMessages2["upstream"] =  bind(&nodes::upstreamMsg, this, placeholders::_1);
  _remoteDataReqMessages2["queryResult"] =  bind(&nodes::sendOnMsg, this, placeholders::_1);
  
  _remoteDataReqMessages["date"] =  bind(&nodes::dateMsg, this, placeholders::_1);
  
  _remoteMsgSubMessages["upd"] =  bind(&nodes::updSubMsg, this, placeholders::_1);
  _remoteMsgSubMessages["mov"] =  bind(&nodes::updSubMsg, this, placeholders::_1); // same handler as upd
  _remoteMsgSubMessages["add"] =  bind(&nodes::addSubMsg, this, placeholders::_1);

  _dataRepMessages2["discoverLocal"] =  bind(&nodes::discoverLocalMsg, this, placeholders::_1);
  _dataRepMessages2["discover"] =  [&](const IncomingMsg &in) {
    sendDownDiscoverResult(in);
  };

  _dataRepMessages["online"] =  bind(&nodes::onlineMsg, this, placeholders::_1);
  _dataRepMessages["heartbeat"] =  bind(&nodes::heartbeatMsg, this, placeholders::_1);

  _dataRepMessages["query"] =  bind(&nodes::queryDrMsg, this, placeholders::_1);
  _dataRepMessages["upd"] =  bind(&nodes::updDrMsg, this, placeholders::_1);
  _dataRepMessages["mov"] =  bind(&nodes::updDrMsg, this, placeholders::_1); // same handler as upd
  _dataRepMessages["add"] =  bind(&nodes::addDrMsg, this, placeholders::_1);
  
  Storage::instance()->init(dbConn, dbName, schema);
  
}
  
Server::~Server() {
}
  
void Server::runUpstreamOnly() {

  L_TRACE("running with upstream only");
  L_INFO("init nodes"); // used to detect server startup
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
//      L_TRACE("not online yet");
    }
    _remoteMsgSub->check();

//    L_DEBUG("polling for messages");
    zmq::message_t message;
    zmq::poll(&items[0], 3, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      if (!getMsg("<-", *_rep, _messages, _messages2)) {
        sendErr("error in getting rep message");
      }
    }
    if (items[1].revents & ZMQ_POLLIN) {
      getMsg("<-rdr", _remoteDataReq->socket(), _remoteDataReqMessages, _remoteDataReqMessages2);
    }
    if (items[2].revents & ZMQ_POLLIN) {
      if (!getMsg("<-ms", _remoteMsgSub->socket(), _remoteMsgSubMessages, nullopt)) {
        sendErr("error in getting upstream rep message");
      }
    }
  }
}

void Server::runUpstreamDownstream() {

  L_TRACE("running with upstream and downstream");
  L_INFO("init nodes"); // used to detect server startup
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
//      L_TRACE("not online yet");
    }
    _remoteMsgSub->check();

//    L_DEBUG("polling for messages");
    zmq::message_t message;
    zmq::poll(&items[0], 4, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      if (!getMsg("<-", *_rep, _messages, _messages2)) {
        sendErr("error in getting rep message");
      }
    }
    if (items[1].revents & ZMQ_POLLIN) {
      getMsg("<-rdr", _remoteDataReq->socket(), _remoteDataReqMessages, _remoteDataReqMessages2);
    }
    if (items[2].revents & ZMQ_POLLIN) {
      if (!getMsg("<-dr", _dataRep->socket(), _dataRepMessages, _dataRepMessages2)) {
        sendErr("error in getting upstream rep message");
      }
    }
    if (items[3].revents & ZMQ_POLLIN) {
      if (!getMsg("<-ms", _remoteMsgSub->socket(), _remoteMsgSubMessages, nullopt)) {
        sendErr("error in getting remote upstream sub message");
      }
    }
  }
}

void Server::runStandalone() {

  L_TRACE("running standalone");
  L_INFO("init nodes"); // used to detect server startup
  zmq::pollitem_t items [] = {
      { *_rep, 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (!_reload) {
  
//    L_DEBUG("polling for messages");
    zmq::message_t message;
    zmq::poll(&items[0], 1, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      if (!getMsg("<-", *_rep, _messages, _messages2)) {
        sendErr("error in getting rep message");
      }
    }
  }
  
}

void Server::runDownstreamOnly() {

  L_TRACE("running downstream only");
  L_INFO("init nodes"); // used to detect server startup
  zmq::pollitem_t items [] = {
      { *_rep, 0, ZMQ_POLLIN, 0 },
      { _dataRep->socket(), 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (!_reload) {
  
//    L_DEBUG("polling for messages");
    zmq::message_t message;
    zmq::poll(&items[0], 2, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      if (!getMsg("<-", *_rep, _messages, _messages2)) {
         sendErr("error in getting rep message");
      }
    }
    if (items[1].revents & ZMQ_POLLIN) {
      if (!getMsg("<-dr", _dataRep->socket(), _dataRepMessages, _dataRepMessages2)) {
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

bool Server::getMsg(const string &name, zmq::socket_t &socket, map<string, msgHandler> &handlers, optional<map<string, msgHandler2> > handlers2) {

  L_TRACE("got " << name << " message");
  zmq::message_t reply;
  try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
    socket.recv(&reply);
#else
    auto res = socket.recv(reply, zmq::recv_flags::none);
#endif
    // convert to JSON
    string r((const char *)reply.data(), reply.size());
    if (handlers2) {
      L_DEBUG("trying new handlers " << name << " " << r);
  
      auto doc = rfl::json::read<IncomingMsg>(r);
      if (!doc) {
        L_ERROR("IncomingMsg missing fields");
        return false;
      }
  
      L_TRACE("handling " << doc->type);
      map<string, msgHandler2>::iterator  handler2 = handlers2->find(doc->type);
      if (handler2 == handlers2->end()) {
        L_TRACE("using old handler");
        auto handler = handlers.find(doc->type);
        if (handler == handlers.end()) {
          L_ERROR("unknown msg type " << doc->type << " for " << name);
          return false;
        }
        else {
          // use the old style doc.
          Data doc(r);
          handler->second(doc);
        }
        return true;
      }
      try {
        handler2->second(*doc);
      }
      catch (exception &exc) {
        L_ERROR("what: " << exc.what());
        L_ERROR("location: " << boost::get_throw_location(exc));
        return false;
      }
      return true;
    }
    
    // no new handlers, do it the old way.
    Data doc(r);

    L_DEBUG(name << " " << doc);

    auto type = Json::getString(doc, "type");
    if (!type) {
      L_ERROR("no type for " << name << " reply");
      return false;
    }

    L_TRACE("handling " << type.value());
    map<string, msgHandler>::iterator handler = handlers.find(type.value());
    if (handler == handlers.end()) {
      L_ERROR("unknown msg type " << type.value() << " for " << name);
      return false;
    }
    try {
      handler->second(doc);
    }
    catch (exception &exc) {
      L_ERROR("what: " << exc.what());
      L_ERROR("location: " << boost::get_throw_location(exc));
      return false;
    }
  }
  catch (zmq::error_t &e) {
    L_WARNING("got exc with " << name << " recv" << e.what() << "(" << e.num() << ")");
  }

  return true;
  
}

void Server::sendTo(zmq::socket_t &socket, const json &j, const string &type, optional<string> corr) {

  json j2 = j;
  if (corr) {
    L_TRACE("had corr id " << corr.value());
    if (type == "&->") {
      // old servers (NodeJS) still use socket id
      j2.as_object()["socketid"] = corr.value();
    }
    j2.as_object()["corr"] = corr.value();
  }

  stringstream ss;
  ss << j2;
  string m = ss.str();
  
  sendTo(socket, m, type);

}

void Server::sendTo(zmq::socket_t &socket, const DictO &in, const string &type, optional<string> corr) {

  DictO in2 = in;
  if (corr) {
    L_TRACE("had corr id " << *corr);
    if (type == "&->") {
      // old servers (NodeJS) still use socket id
      in2["socketid"] = *corr;
    }
    in2["corr"] = *corr;
  }

  sendTo(socket, Dict::toString(in2), type);

}

void Server::sendTo(zmq::socket_t &socket, const string &m, const string &type) {

  L_DEBUG(type << " " << m);

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
    L_WARNING("got exc publish" << e.what() << "(" << e.num() << ")");
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

void Server::setSrc(DictO *m) {

  if (!Storage::appendArray(m, "path", _serverId)) {
    L_ERROR(_serverId << " is already in the path!");
  }
  
}

void Server::setSrc(boost::json::object *m) {

  if (!Json::appendArray(m, "path", _serverId)) {
    L_ERROR(_serverId << " is already in the path!");
  }
  
}

bool Server::getSrc(const IncomingMsg &in, string *s) {

  if (!in.path) {
    L_ERROR("missing path in msg");
    return false;
  }
  if (in.path->size() == 0) {
    L_ERROR("path is empty in message");
    return false;
  }
  *s = in.path->back();
  return true;

}

bool Server::getSrc(json &msg, string *s) {

  return Json::arrayTail(msg.as_object(), "path", s);

}

bool Server::wasFromUs(const DictO &msg) {

  // make sure it's not on the path either.
  return Storage::arrayHas(msg, "path", _serverId);
  
}

bool Server::wasFromUs(const Data &msg) {

  // make sure it's not on the path either.
  return Json::arrayHas(msg.as_object(), "path", _serverId);
  
}

void Server::sendDown(const DictO &m) {

  sendTo(_dataRep->socket(), m, "dr-> ", nullopt);
  
}

void Server::sendDown(const std::string &m) {

  sendTo(_dataRep->socket(), m, "dr-> ");
  
}

void Server::pubDown(const DictO &m) {

  // no destination for publishes.
  DictO m2 = m;
  // TBD: how do we do this with copy_if etc.
  for (auto i: m) {
    if (get<0>(i) != "dest") {
      m2[get<0>(i)] = get<1>(i);
    }
  }
  
  sendTo(_msgPub->socket(), m2, "mp-> ", nullopt);

}

void Server::pubDown(const json &m) {

  // no destination for publishes.
  boost::json::object m2 = m.as_object();
  m2.erase("dest");
  
  sendTo(_msgPub->socket(), m2, "mp-> ", nullopt);

}

void Server::sendOn(const DictO &origm) {

  auto data = Dict::getObject(origm, "data");
  if (!data) {
    L_ERROR("sendOn message has no data");
    return;
  }
  auto type = Dict::getString(data.value(), "type");
  if (!type) {
    L_ERROR("sendOn message data has no type");
    return;
  }
  auto obj = Dict::getObject(data.value(), "obj");
  if (!obj) {
    L_ERROR("sendOn message data has no obj");
    return;
  }
  
  auto msg = origm;
  setSrc(&msg);

  if (hasUpstream() && (isObjUpstream(obj.value()) || isObjParentUpstream(type.value(), obj.value()))) {
    L_TRACE("sendOn");
    msg["dest"] = _upstreamId;
    sendDataReq(nullopt, msg);
  }
  
  // try to find the id. It's either in the object or in the message data
  auto id = Dict::getString(data.value(), "id");
  if (!id) {
    id = Dict::getString(obj.value(), "id");
  }
  if (!id) {
    L_ERROR("sendOn message data and obj has no id");
    return;
  }
  
  if (hasValidNodes() && shouldSendDown("on", type.value(), id.value(), obj.value())) {
    L_TRACE("publish down");
    pubDown(msg);
  }
}

string Server::buildErrJson(const string &level, const string &msg) {

  L_TRACE("buildErrJson " << level << ", " << msg);
  
  struct ErrMsg {
    string type = "err";
    string level;
    string msg;
  } m{};
  m.level = level;
  m.msg = msg;
  return rfl::json::write(m, rfl::json::pretty);
  
}

string Server::buildAckJson(optional<string> result) {

  // it might look ok to simply assign the result but
  // that doesn't work. We must explicitly do it.
  struct AckMsg {
    string type = "ack";
    optional<string> result;
  } m{};
  if (result) {
    m.result = *result;
  }
  return rfl::json::write(m, rfl::json::pretty);
  
}

string Server::buildCollResultJson(const IncomingMsg &in, const string &name, const DictV &array) {

  struct CollResult {
    string type;
    struct {
      optional<bool> latest;
      optional<long> time;
    } test;
    // the collection name is variable.
    rfl::ExtraFields<DictV> extra_fields;
  };
  
  if (testCollectionChanged(in, name)) {
    CollResult m{
      .type = name, 
      .test={
        .latest = true
      }
    };
    return rfl::json::write(m, rfl::json::pretty);
  }

  CollResult m{
    .type = name, 
    .test={
      .time = Storage::instance()->collectionChanged(name)
    }
  };
  m.extra_fields[name] = array; 
  return rfl::json::write(m, rfl::json::pretty);
  
}

string Server::buildObjResultJson(const IncomingMsg &in, const string &name, const DictG &obj) {

  struct ObjResult {
    string type;
    struct {
      optional<bool> latest;
    } test;
    // the collection name is variable.
    rfl::ExtraFields<DictG> extra_fields;
  };
  
  if (testModifyDate(in, obj)) {
    ObjResult m{
      .type = name, 
      .test={
        .latest = true
      }
    };
    return rfl::json::write(m, rfl::json::pretty);
  }

  ObjResult m{
    .type = name
  };
  m.extra_fields[name] = obj; 
  return rfl::json::write(m, rfl::json::pretty);
  
}

void Server::sendErr(const string &msg) {

  L_ERROR(msg);
  send(buildErrJson("fatal", msg));
  
}

void Server::sendErrDown(const string &msg) {

  L_ERROR(msg);
  sendDown(buildErrJson("fatal", msg));
  
}

void Server::sendWarning(const string &msg) {

  L_ERROR(msg);
  send(buildErrJson("warning", msg));
  
}

void Server::sendAck(optional<string> result) {

  send(buildAckJson(result));
  
}

void Server::sendAckDown(optional<string> result) {

  sendDown(buildAckJson(result));
  
}

void Server::sendSecurity() {

  L_ERROR("security error");
  send(buildErrJson("security", ""));
  
}

bool Server::testModifyDate(json &j, const json &doc) {

  auto test = Json::getObject(j, "test", true);
  if (test) {
    auto time = Json::getString(test.value(), "time", true);
    if (time) {
      long mod = Date::fromISODate(Json::getString(doc, "modifyDate").value());
      long t = Date::fromISODate(time.value());
      if (mod <= t) {
        L_TRACE("not changed " << mod << " <= " << t);
        return true;
      }
    }
  }
  return false;
}

bool Server::testModifyDate(const IncomingMsg &m, const DictG &obj) {

  if (m.test) {
    if (m.test->modifyDate) {
      auto modDate = Dict::getStringG(obj, "modifyDate");
      if (!modDate) {
        L_ERROR("modifyDate missing");
        return false;
      }
      long mod = Date::fromISODate(*modDate);
      long t = Date::fromISODate(*(m.test->modifyDate));
      if (mod <= t) {
        L_TRACE("not changed " << mod << " <= " << t);
        return true;
      }
    }
  }
  return false;
}

bool Server::testCollectionChanged(const IncomingMsg &m, const string &name) {

  if (m.test) {
    if (m.test->time) {
      long changed = Storage::instance()->collectionChanged(name);
      if (changed <= *(m.test->time)) {
        L_TRACE("not changed " << changed << " <= " << *(m.test->time));
        return true;
      }
    } 
  }
  return false;
}

void Server::sendCollection(const IncomingMsg &m, const string &name, const DictV &array) {

  send(buildCollResultJson(m, name, array));

}

void Server::sendObject(const IncomingMsg &m, const string &name, const DictG &obj) {

  send(buildObjResultJson(m, name, obj));

}

void Server::sendDataReq(optional<string> corr, const DictO &m) {

  sendTo(_remoteDataReq->socket(), m, "rdr-> ", corr);
  
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

  Info().deleteMany(dictO({{ 
    "type", dictO({{ 
      "$in", DictV({"upstream", "upstreamPubKey", "hasInitialSync", "upstreamLastSeen"})
    }}) 
  }}));

  systemStatus("Server orphaned");
  stopUpstream();

}

void Server::connectUpstream() {

  int major, minor, patch;
  zmq_version(&major, &minor, &patch);
  L_INFO("zmq version " << major << "." << minor << "." << patch);
  
  if (!zmq_has("curve")) {
    L_INFO("no curve available");
    return;
  }

  stopUpstream();
  
  auto docs = Info().find(dictO({{ 
    "type", dictO({{ 
      "$in", DictV({"serverId", "upstream", "upstreamPubKey", "privateKey", "pubKey"}) 
    }})
  }}), {"type", "text"}).all();
  if (!docs) {
    L_INFO("no infos.");
    return;
  }
  
  auto privateKey = Info::getInfo(docs.value(), "privateKey");
  if (!privateKey) {
    L_ERROR("upstream, but no privateKey");
    return;
  }

  if (_dataRepPort && _msgPubPort) {
    _msgPub.reset(new Downstream(this, *_context, ZMQ_PUB, "msgPub", _msgPubPort, privateKey.value()));
    _dataRep.reset(new Downstream(this, *_context, ZMQ_REP, "dataRep", _dataRepPort, privateKey.value()));
  }

  auto serverId = Info::getInfo(docs.value(), "serverId");
  if (!serverId) {
    L_INFO("no serverId.");
    return;
  }
  _serverId = serverId.value();

  auto upstream = Info::getInfo(docs.value(), "upstream");
  if (!upstream) {
    L_INFO("no upstream.");
    return;
  }
  auto upstreamPubKey = Info::getInfo(docs.value(), "upstreamPubKey");
  if (!upstreamPubKey) {
    L_ERROR("upstream, but no upstreamPubKey");
    return;
  }
  auto pubKey = Info::getInfo(docs.value(), "pubKey");
  if (!pubKey) {
    L_ERROR("upstream, but no pubKey");
    return;
  }
  
  _pubKey =  pubKey.value();
  
	L_TRACE("upstream: " << upstream.value());
	L_TRACE("upstreamPubKey: " << upstreamPubKey.value());
	L_TRACE("privateKey: " << privateKey.value());
	L_TRACE("_pubKey: " << _pubKey);

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

  L_TRACE("online");

  auto doc = Site().find(dictO({}), {}).one();
  if (!doc) {
    L_INFO("no site.");
    return;
  }
  
  auto infos = Info().find(dictO({{ 
    "type", dictO({{ 
      "$in", DictV({"hasInitialSync", "upstreamMirror"})
    }}) 
  }}), {"type", "text"}).all();
  string hasInitialSync = Info::getInfoSafe(infos, "hasInitialSync", "false");
  string upstreamMirror = Info::getInfoSafe(infos, "upstreamMirror", "false");

  DictO msg = dictO({
    { "type", "online" },
    { "build", "28474" },
    { "headerTitle", doc.value().headerTitle() },
    { "streamBgColor", doc.value().streamBgColor() },
    { "pubKey", _pubKey },
    { "synced", hasInitialSync == "true" },
    { "mirror", upstreamMirror == "true" }
  });
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
  
  DictO msg = dictO({
    { "type", "heartbeat" }
  });
  setSrc(&msg);  
	sendDataReq(nullopt, msg);
	
}

bool Server::setInfo(const string &name, const string &text) {

  auto doc = Info().find(dictO({{ "type", name }}), {"text"}).one();
  if (doc) {
    L_TRACE("info old value " << Dict::toString(doc.value().dict()));
    auto result = Info().update(dictO({{ "type", name }}), dictO({{ "text", text }}));
    if (!result) {
      L_ERROR("could not update info");
      return false;
    }
    L_TRACE("updated " << result.value());
    return true;
  }

  auto result = Info().insert(dictO({
    { "type", name },
    { "text", text }
  }));
  if (!result) {
    L_ERROR("could not insert info");
    return false;
  }
  L_TRACE("inserted " << result.value());
  return true;
  
}

bool Server::resetServer() {
  
  char pubkey[41];
  char seckey[41];
  int rc = zmq_curve_keypair(pubkey, seckey);
  if (rc != 0) {
    L_ERROR("failed to generate key pair " << rc);
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
  Info().deleteMany(dictO({{ 
    "type", dictO({{ 
      "$in", DictV({"upstream", "upstreamPubKey", "upstreamMirror", "hasInitialSync", "upstreamLastSeen" })
    }})
  }}));
  
  // make sure to start security again.
  Security::rebuild();
  
  return true;
}

string Server::get1Info(const string &type) {

  auto docs = Info().find(dictO({{ "type", type }})).all();
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

  L_INFO(msg);
  
  publish(nullopt, dictO({
    { "type", "status" },
    { "text", msg }
  }));
  
}

string Server::zeroDate() {

  // return a date representing the start of all time.
  return Date::toISODate(0);
  
}

string Server::getLastDate(optional<DictV> rows, const string &hasInitialSync, const string &date, bool all) {

  L_TRACE("getLastDate rows " << (rows ? rows.value().size() : 0) << " all " << all);
  
  string zd = zeroDate();
  
  if (rows && rows.value().size() > 0) {
    if (hasInitialSync == "true" || all) {
      // does any of the rows have a null date?
      bool hasNullDate = find_if(rows.value().begin(), rows.value().end(), [zd](auto e) { 
        return Dict::getStringG(e, "modifyDate") == zd;
      }) != rows.value().end();
      if (hasNullDate) {
        return zd;
      }
      auto d = Dict::getStringG(*(rows.value().begin()), "modifyDate");
      if (d) {
        L_TRACE("returning " << d.value());
        return d.value();
      }
    }
    L_TRACE("returning zd");
    return zd;
  }
  
  // no rows
  return (hasInitialSync == "true") ? date : zd;
  
}

string Server::getCollName(const string &type, optional<string> coll) {

  return coll ? coll.value() : (type + "s");
  
}

optional<DictV> Server::getSubobjsLatest(const DictO &subobj, const vector<string> &ids, const string &hasInitialSync, const string &upstreamLastSeen, bool collzd) {

  L_TRACE("getSubobjsLatest");
  
  auto type = Dict::getString(subobj, "type");
  if (!type) {
    L_ERROR("type missing in schema obj " << Dict::toString(subobj));
    return {};
  }
  string collname = getCollName(type.value(), Dict::getString(subobj, "coll"));
  auto field = Dict::getString(subobj, "field");
  DictV d;
  for (auto id: ids) {
    // make sure to ignore all locally new objects when getting the latest.
    DictO q = dictO({{ "localNew", dictO({{ "$ne", true }})}});
    if (id != "*") {
      q[field.value()] = id;
    }
    auto objs = SchemaImpl::findGeneral(collname, q, { "_id", "modifyDate" }, 1, dictO({{ "modifyDate", -1 }}));
    if (!objs) {
      L_ERROR("couldn't find subobjs");
      return {};
    }
    string lastseen = getLastDate(objs->all(), hasInitialSync, collzd ? zeroDate() : upstreamLastSeen, id == "*");
    string lastname = type.value();
    lastname[0] = toupper(lastname[0]);
    lastname = "last" + lastname;
    d.push_back(dictO({
      { "id", id },
      { lastname, lastseen }
    }));
  }
  
  L_TRACE("returning " << d.size() << " objs");
  return d;
  
}

void Server::sendUpDiscover() {

  auto infos = Info().find(dictO({{ 
    "type", dictO({{ 
      "$in", DictV({"hasInitialSync", "upstreamLastSeen", "upstreamMirror"})
    }})
  }}), {"type", "text"}).all();
  string hasInitialSync = Info::getInfoSafe(infos, "hasInitialSync", "false");
  string upstreamLastSeen = Info::getInfoSafe(infos, "upstreamLastSeen", "");
  string upstreamMirror = Info::getInfoSafe(infos, "upstreamMirror", "");
  
  DictO msg = dictO({
    { "type", "discover" },
    { "hasInitialSync", hasInitialSync == "true" }
  });

  for (auto o: Storage::instance()->_schema) {
    auto type = Dict::getString(o, "type");
    if (!type) {
      L_ERROR("type missing in schema obj " << Dict::toString(o));
      return;
    }
    
    string collname = getCollName(type.value(), Dict::getString(o, "coll"));
    string lastname = type.value();
    lastname[0] = toupper(lastname[0]);
    lastname = "last" + lastname;

    vector<string> ids;
    string colldate;
    if (upstreamMirror == "true") {
      // "*" means ALL objects.
      ids.push_back("*");
      colldate = getLastDate(nullopt, hasInitialSync, upstreamLastSeen, true);
    }
    else {
      auto objs = SchemaImpl::findGeneral(collname, dictO({{ "upstream", true }}), { "_id", "modifyDate" }, nullopt)->all();
      if (objs) {
        transform(objs.value().begin(), objs.value().end(), back_inserter(ids), [](auto e) {
          auto s = Dict::getStringG(e, "id");
          if (s) {
            return *s;
          }
          return string("??");
        });
      }
      colldate = getLastDate(objs, hasInitialSync, upstreamLastSeen, false);
    }
    
    msg[lastname] = colldate;
    
    auto subobj = Dict::getObject(o, "subobj");
    if (subobj) {
      auto d = getSubobjsLatest(subobj.value(), ids, hasInitialSync, upstreamLastSeen, colldate == zeroDate());
      if (!d) {
        return;
      }
      msg[collname] =  d.value();
    }
    else {
      DictV v;
      transform(ids.begin(), ids.end(), back_inserter(v), [](auto e) {
        return DictG(e);
      });
      msg[collname] = v;
    }
  }
  setSrc(&msg);
  
	sendDataReq(nullopt, msg);

}

void Server::unmarkAll(const DictO &obj) {

  auto type = Dict::getString(obj, "type");
  if (!type) {
    L_ERROR("type missing in schema obj " << Dict::toString(obj));
    return;
  }
  string collname = getCollName(type.value(), Dict::getString(obj, "coll"));

  if (!SchemaImpl::updateGeneral(collname, 
      dictO({{ "localNew", true }}), 
      dictO({{ "$unset", dictO({{ "localNew", "" }}) }}))) {
    L_ERROR("unable to unmark object");
  }

}

void Server::discoveryComplete() {
  
  systemStatus("Discovery complete");
  
  // make sure everything is regenerated
  Security::instance()->regenerateGroups();
  Security::instance()->regenerate();
  
  // noce to have a pattern for this.
  for (auto o: Storage::instance()->_schema) {
    unmarkAll(o);
    auto subobj = Dict::getObject(o, "subobj");
    if (subobj) {
      unmarkAll(subobj.value());
    }
  }
  
}

#ifdef MONGO_DB
bool Server::collectObjs(const string &type, const string &collname, bsoncxx::document::view_or_value q, 
    DictV *data, vector<string> *policies, optional<int> limit, bool mark) {

  // fetch 1 more than the limit.
  optional<int> fetchlimit;
  if (limit) {
    fetchlimit = limit.value() + 1;
  }
  
  bool more = false;
  auto result = SchemaImpl::findGeneral(collname, q, {}, fetchlimit, dictO({{ "modifyDate", 1 }}));
  if (result) {
    auto vals = result->all();
    if (vals) {
      auto val = vals.value();
      if (limit && val.size() > limit.value()) {
        more = true;
        // remove the last 1 since only 1 extra
        val.pop_back();
      }
      DictO obj = dictO({
        { "type", type },
        { "objs", val }
      });
      data->push_back(obj);
      
      // collect all the policies of the objects, and mark if necessary
      for (auto i: val) {
        if (mark) {
          // we mark all objects to ignore when querying locally for changes.
          if (!SchemaImpl::updateGeneralById(collname, Dict::getStringG(i, "id").value(), dictO({{ "$set", dictO({{ "localNew", true }}) }}))) {
            L_ERROR("unable to mark object");
          }
        }
        auto p = Dict::getStringG(i, "policy");
        if (p) {
          if (find(policies->begin(), policies->end(), p.value()) == policies->end()) {
            policies->push_back(p.value());
          }
        }
      }
    }
  }

  return more;
  
}
#endif

void Server::collectPolicies(const vector<string> &policies, DictV *data) {

#ifdef MONGO_DB
  auto q = SchemaImpl::idRangeQuery(policies);
  auto result = SchemaImpl::findGeneral("policies", q, {});
  if (result) {
    auto vals = result->all();
    if (vals) {
      DictO obj = dictO({
        { "type", "policy" },
        { "objs", vals.value() }
      });
      data->push_back(obj);
    }
  }
#endif

}

void Server::sendUpDiscoverLocalUpstream(const string &upstreamLastSeen, optional<string> corr) {
    
  L_TRACE("sendUpDiscoverLocalUpstream");
  
#ifdef MONGO_DB
  auto q = SchemaImpl::boolFieldEqualAfterDateQuery("upstream", true, upstreamLastSeen);

  DictV data;
  vector<string> policies;
  for (auto o: Storage::instance()->_schema) {
  
    auto nosync = Dict::getBool(o, "nosync");
    if (nosync && nosync.value()) {
      continue;
    }
    
    auto type = Dict::getString(o, "type");
    if (!type) {
      L_ERROR("type missing in schema obj " << Dict::toString(o));
      return;
    }
    
    string collname = getCollName(type.value(), Dict::getString(o, "coll"));
    
    collectObjs(type.value(), collname, q, &data, &policies, nullopt, false);
    
    auto subobj = Dict::getObject(o, "subobj");
    if (subobj) {
      auto field = Dict::getString(subobj.value(), "field");
      if (!field) {
        L_ERROR("field missing in schema subobj " << Dict::toString(subobj.value()));
        return;
      }
      auto result = SchemaImpl::findGeneral(collname, dictO({{ "upstream", true }}), { "_id" });
      if (result) {
        auto upstreams = result->all();
        if (upstreams) {
          auto subtype = Dict::getString(subobj.value(), "type");
          if (!subtype) {
            L_ERROR("type missing in schema subobj " << Dict::toString(subobj.value()));
            return;
          }
          string subcollname = getCollName(subtype.value(), Dict::getString(subobj.value(), "coll"));
          for (auto o: upstreams.value()) {
            auto id = Dict::getStringG(o, "id");
            collectObjs(subtype.value(), subcollname, 
              SchemaImpl::stringFieldEqualAfterDateQuery(field.value(), id.value(), upstreamLastSeen), &data, &policies, nullopt, true);
          }
        }
      }
    }
  }
  
  // we don't send policies upstream.
  // that's why the policies array is ignored.
  DictO msg = dictO({
    { "type", "discoverLocal" },
    { "data", data }
  });
  setSrc(&msg);  
  sendDataReq(corr, msg);

#endif
  
}

void Server::sendUpDiscoverLocalMirror(const string &upstreamLastSeen, optional<string> corr) {
    
  L_TRACE("sendUpDiscoverLocalMirror");

#ifdef MONGO_DB
  auto q = SchemaImpl::afterDateQuery(upstreamLastSeen);

  DictV data;
  vector<string> policies;
  for (auto o: Storage::instance()->_schema) {
  
    auto nosync = Dict::getBool(o, "nosync");
    if (nosync && nosync.value()) {
      continue;
    }
    
    auto type = Dict::getString(o, "type");
    if (!type) {
      L_ERROR("type missing in schema obj " << Dict::toString(o));
      return;
    }
    
    string collname = getCollName(type.value(), Dict::getString(o, "coll"));
    
    collectObjs(type.value(), collname, q, &data, &policies, nullopt, false);
    
    auto subobj = Dict::getObject(o, "subobj");
    if (subobj) {
      auto field = Dict::getString(subobj.value(), "field");
      if (!field) {
        L_ERROR("field missing in schema subobj " << Dict::toString(subobj.value()));
        return;
      }
      auto result = SchemaImpl::findGeneral(collname, dictO({}), { "_id" });
      if (result) {
        auto upstreams = result->all();
        if (upstreams) {
          auto subtype = Dict::getString(subobj.value(), "type");
          if (!subtype) {
            L_ERROR("type missing in schema subobj " << Dict::toString(subobj.value()));
            return;
          }
          string subcollname = getCollName(subtype.value(), Dict::getString(subobj.value(), "coll"));
          for (auto o: upstreams.value()) {
            auto id = Dict::getStringG(o, "id");
            collectObjs(subtype.value(), subcollname, SchemaImpl::afterDateQuery(upstreamLastSeen), &data, &policies, nullopt, false);
          }
        }
      }
    }
  }
  
  collectPolicies(policies, &data);

  DictO msg = dictO({
    { "type", "discoverLocal" },
    { "data", data }
  });
  setSrc(&msg);  
  sendDataReq(corr, msg);
#endif
}

void Server::sendUpDiscoverLocal(optional<string> corr) {

  auto infos = Info().find(dictO({{ 
    "type", dictO({{ 
      "$in", DictV({"hasInitialSync", "upstreamLastSeen", "upstreamMirror"})
    }} )
  }}), {"type", "text"}).all();
  string hasInitialSync = Info::getInfoSafe(infos, "hasInitialSync", "false");
  string upstreamLastSeen = Info::getInfoSafe(infos, "upstreamLastSeen", "");
  string upstreamMirror = Info::getInfoSafe(infos, "upstreamMirror", "");
  
  if (hasInitialSync != "true") {
    L_TRACE("no initial sync, nothing to discover locally.");
    DictV empty;
    DictO msg = dictO({
      { "type", "discoverLocal" },
      { "data", empty }
    });
    setSrc(&msg);  
    sendDataReq(corr, msg);
    return;
  }
  if (upstreamLastSeen == "") {
    L_ERROR("initial sync, but no upstreamLastSeen?");
    return;
  }

  // use the schema to 
  L_TRACE("upstream since " << upstreamLastSeen);
  
  if (upstreamMirror == "true") {
    sendUpDiscoverLocalMirror(upstreamLastSeen, corr);
  }
  else {
    sendUpDiscoverLocalUpstream(upstreamLastSeen, corr);
  }
}

void Server::collectIds(const DictV &ids, vector<string> *vids) {

  // convert all the user ids to oids.
  // ids are an array of strings or an array of objects with "id"
  for  (auto u: ids) {
    auto s = Dict::getString(u);
    if (s) {
      vids->push_back(*s);
    }
    else {
      auto o = Dict::getObject(u);
      if (o) {
        auto id = Dict::getString(o, "id");
        if (!id) {
          L_ERROR("idRangeAfterDateQuery id missing from obj or not a string");
          continue;
        }
        vids->push_back(*id);
      }
    }
  }
  
}

void Server::sendDownDiscoverResult(const IncomingMsg &in) {

#ifdef MONGO_DB
  string src;
  if (!getSrc(in, &src)) {
    sendErrDown("discover missing src");
    return;
  }

  auto node = Node().find(dictO({ { "serverId", src } }), {}).one();
  if (!node) {
    sendErrDown("discover no node");
    return;
  }
  
  if (!node.value().valid()) {
    sendErrDown("discover node invalid");
    return;
  }

  bool hasMore = false;
  int limit = MAX_OBJECTS;
    
  DictV msgs;
  DictO obj;
  vector<string> policies;
  for (auto o: Storage::instance()->_schema) {
  
    auto nosync = Dict::getBool(o, "nosync");
    if (nosync && nosync.value()) {
      continue;
    }
    
    auto type = Dict::getString(o, "type");
    if (!type) {
      L_ERROR("type missing in schema obj " << Dict::toString(o));
      return;
    }
    
    string collname = getCollName(type.value(), Dict::getString(o, "coll"));

    string lastname = type.value();
    lastname[0] = toupper(lastname[0]);
    lastname = "last" + lastname;
    L_TRACE(lastname);
    
    auto objsname = collname;
    L_TRACE(objsname);

    auto last = Dict::getString(in.extra_fields.get(lastname));
    auto objs = Dict::getVector(in.extra_fields.get(objsname));
    if (last) {
      obj[lastname] = last.value();
    }
    if (objs) {
      obj[objsname] = objs.value();
    }
    
    if (last && objs && objs.value().size() > 0) {
    
      // collect all the ids.
      vector<string> ids;
      collectIds(objs.value(), &ids);
      
      // collect all the objects 
      bool more = collectObjs(type.value(), collname, SchemaImpl::idRangeAfterDateQuery(ids, last.value()), &msgs, &policies, limit, false);
      if (more) {
        hasMore = true;
      }
      auto subobj = Dict::getObject(o, "subobj");
      if (subobj) {
        auto field = Dict::getString(subobj.value(), "field");
        if (!field) {
          L_ERROR("field missing in schema subobj " << Dict::toString(subobj.value()));
          return;
        }
        auto subtype = Dict::getString(subobj.value(), "type");
        if (!subtype) {
          L_ERROR("type missing in schema subobj " << Dict::toString(subobj.value()));
          return;
        }
        string lastname = subtype.value();
        lastname[0] = toupper(lastname[0]);
        lastname = "last" + lastname;
        L_TRACE(lastname);
        string subcollname = getCollName(subtype.value(), Dict::getString(subobj.value(), "coll"));
        if (ids.size() == 1 && *(ids.begin()) == "*") {
          auto last = Dict::getStringG(*(objs.value().begin()), lastname).value();
          more = collectObjs(subtype.value(), subcollname, 
            SchemaImpl::afterDateQuery(last), &msgs, &policies, limit, false);
          if (more) {
            hasMore = true;
          }
        }
        else {
          for (auto o: objs.value()) {
            auto id = Dict::getStringG(o, "id").value();
            auto last = Dict::getStringG(o, lastname).value();
            L_TRACE(id << " " << last);
            more = collectObjs(subtype.value(), subcollname, 
              SchemaImpl::stringFieldEqualAfterDateQuery(field.value(), id, last), &msgs, &policies, limit, false);
            if (more) {
              hasMore = true;
            }
          }
        }
      }
    }
  
  }

  collectPolicies(policies, &msgs);
  
  // update the node where we are.  
  Node().updateById(node.value().id(), obj);

  DictO msg = dictO({
    { "type", "discoverResult" },
    { "msgs", msgs },
  });
  
  if (hasMore) {
    msg["more"] = true;
  }
  // and send the result on.
  sendDown(msg);
#endif
}

void Server::resetDB() {
 
  L_INFO("DB reset");
  Storage::instance()->allCollectionsChanged();
   
}

void Server::importObjs(const DictV &msgs) {

  for (auto a: msgs) {
  
    auto type = Dict::getStringG(a, "type");
    if (!type) {
      L_ERROR("msg missing type");
      continue;
    }
    auto objs = Dict::getVectorG(a, "objs");
    if (!objs) {
      L_ERROR("msg missing objs");
      continue;
    }
    
    string coll;
    if (!Storage::instance()->collName(type.value(), &coll, false)) {
      continue;
    }
    vector<DictO> v;
    transform(objs->begin(), objs->end(), back_inserter(v), [](auto e) {
      auto o = Dict::getObject(e);
      if (!o) {
        L_ERROR("object isn't an object");
        return DictO();
      }
      return *o;
    });
    Storage::instance()->bulkInsert(coll, v);

    auto more = Dict::getBoolG(a, "more");
    if (more && more.value()) {
      L_INFO("ignoring more for " << type.value());
      continue;
    }
  }

}

bool Server::isValidId(const string &id) {
  return id.size() == 24;
}

bool Server::validateId(const DictO &obj, const string &id) {

  auto objid = Dict::getString(obj, "id");
  if (!objid) {
    return true;
  }
  if (objid.value() != id) {
    L_TRACE("skipping update, ids don't match " << objid.value() << " " << id);
    return false;
  }
  return true;
}

vector<string> Server::getNodeIds(const string &type) {

  auto nodes = Node().find(dictO({{ "valid", true }})).all();
  if (!nodes || nodes.value().size() == 0) {
    return {};
  }
  string field = type + "s";
  vector<string> ids;
  for (auto n: nodes.value()) {
    auto a = Dict::getVector(n.dict(), field);
    if (!a) {
      L_ERROR("node missing ids for " << field);
      continue;
    }
    vector<string> nids;
    collectIds(*a, &nids);
    for (auto id: nids) {
      if (find(ids.begin(), ids.end(), id) == ids.end()) {
        ids.push_back(id);
      }
    }
  }
  L_TRACE("getNodeIds " << boost::algorithm::join(ids, ", "));
  
  return ids;

}

bool Server::hasValidNodes() {

  L_TRACE("hasValidNodes");

  if (_dataRep) {
    auto nodes = Node().find(dictO({{ "valid", true }})).all();
    if (!nodes || nodes.value().size() == 0) {
      L_TRACE("skipping send down mov nobody listening at all");
      return false;
    }
    return true;
  }
  
  return false;
  
}

bool Server::anyNodesListening(const string &type, const string &id) {

  L_TRACE("anyNodesListening");

  // search for it.
  auto ids = getNodeIds(type);
  if (find(ids.begin(), ids.end(), "*") != ids.end() || find(ids.begin(), ids.end(), id) != ids.end()) {
    return true;
  }
  L_TRACE("skipping mov nobody listening " << type << " " << id);
  return false;

}

bool Server::shouldSendDown(const string &action, const string &type, const string &id, const DictO &obj) {

  L_TRACE("shouldSendDown");

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
    auto iparent = Dict::getString(obj, pfield);
    if (!iparent) {
      L_ERROR("shouldSendDown obj has no parent field");
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

bool Server::isObjParentUpstream(const string &type, const DictO &obj) {

  L_TRACE("isObjParentUpstream");

  string ptype;
  string pfield;
  if (Storage::instance()->parentInfo(type, &pfield, &ptype)) {
    auto iparent = Dict::getString(obj, pfield);
    if (!iparent) {
      L_ERROR("obj is missing " << pfield);
      return false;
    }
    string coll;
    if (!Storage::instance()->collName(ptype, &coll)) {
      return false;
    }

    auto result = SchemaImpl::findByIdGeneral(coll, iparent.value(), { "upstream" });
    if (!result) {
      L_ERROR("Can't find " << iparent.value());
      return false;
    }
    
    auto s = result->value();
    if (s) {
      auto upstream = Dict::getBool(s.value(), "upstream");
      return upstream && upstream.value();
    }
  }
  return false;

}

bool Server::isParentUpstream(const string &ptype, const string &origparent) {

  L_TRACE("isParentUpstream");

  string coll;
  if (!Storage::instance()->collName(ptype, &coll)) {
    return false;
  }
  
  auto result = SchemaImpl::findByIdGeneral(coll, origparent, { "upstream" });
  if (!result) {
    L_ERROR("Can't find " << origparent);
    return false;
  }
  auto s = result->value();
  if (s) {
    auto upstream = Dict::getBool(s.value(), "upstream");
    return upstream && upstream.value();
  }

  L_TRACE("not sending up");
  
  return false;
}

bool Server::hasUpstream() {

  L_TRACE("hasUpstream " << _upstreamId);

  return _upstreamId != "";
  
}

bool Server::isObjUpstream(const DictO &obj) {

  L_TRACE("isObjUpstream");

  string mirror = get1Info("upstreamMirror");
  if (mirror == "true") {
    L_TRACE("mirror sending up");
    return true;
  }
  auto upstream = Dict::getBool(obj, "upstream");
  if (upstream && upstream.value()) {
    L_TRACE("upstream sending up");
    return true;
  }

  return false;
  
}

void Server::sendUpd(const string &type, const string &id, const DictO &obj) {

  L_TRACE("sendUpd " << type);

  if (!isValidId(id)) {
    L_WARNING("skipping upd, obj id is not valid" << id);
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
    L_TRACE("not sending up or down");
    return;
  }
  
  if (obj.empty()) {
    return;
  }

  DictO msg = dictO({
    { "type", "upd" },
    { "data", dictO({
      { "type", type },
      { "id", id },
      { "obj", obj }
      })
    }
  });
  setSrc(&msg);
  if (down) {
    pubDown(msg);
  }
  msg["dest"] = _upstreamId;
  if (up) {
    sendDataReq(nullopt, msg);
  }

}

void Server::sendAdd(const string &type, const DictO &obj) {

  L_TRACE("sendAdd " << type);

  auto id = Dict::getString(obj, "id");
  if (!id) {
    L_WARNING("skipping add, but obj id ");
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
    L_TRACE("not sending");
    return;
  }
  
  if (obj.empty()) {
    return;
  }

  DictO msg = dictO({
    { "type", "add" },
    { "data", dictO({
      { "type", type },
      { "obj", obj }
      })
    }
  });
  
  setSrc(&msg);
  if (down) {
    pubDown(msg);
  }
  msg["dest"] = _upstreamId;
  if (up) {
    sendDataReq(nullopt, msg);
  }

}

void Server::sendMov(const string &type, const string &id, const DictO &obj, const string &ptype, const string &origparent) {

  L_TRACE("sendMov" << type);

  if (!isValidId(id)) {
    L_WARNING("skipping mov, obj id is not valid" << id);
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
    L_TRACE("not sending up or down");
    return;
  }
  
  if (obj.empty()) {
    return;
  }

  DictO msg = dictO({
    { "type", "mov" },
    { "data", dictO({
      { "type", type },
      { "id", id },
      { "obj", obj },
      { "orig", origparent }
      })
    }
  });
  setSrc(&msg);
  if (down) {
    pubDown(msg);
  }
  msg["dest"] = _upstreamId;
  if (up) {
    sendDataReq(nullopt, msg);
  }

}

bool Server::updateObject(const DictO &j) {

  auto data = Dict::getObject(j, "data");
  if (!data) {
    L_ERROR("upd sub missing data");
    return false;
  }
  auto type = Dict::getString(data.value(), "type");
  if (!type) {
    L_ERROR("upd sub data missing type");
    return false;
  }
  auto id = Dict::getString(data.value(), "id");
  if (!id) {
    L_ERROR("upd sub data missing id");
    return false;
  }
  auto obj = Dict::getObject(data.value(), "obj");
  if (!obj) {
    L_ERROR("upd sub data missing obj");
    return false;
  }
  
  string coll;
  if (!Storage::instance()->collName(type.value(), &coll, false)) {
    return false;
  }

  auto result = SchemaImpl::updateGeneralById(coll, id.value(), dictO({{ "$set", obj.value() }}));
  if (!result) {
    L_ERROR("upd sub failed to update db");
    return false;
  }

  return true;
  
}

bool Server::addObject(const DictO &j) {

  auto data = Dict::getObject(j, "data");
  if (!data) {
    L_ERROR("add sub missing data");
    return false;
  }
  auto type = Dict::getString(data.value(), "type");
  if (!type) {
    L_ERROR("add sub data missing type");
    return false;
  }
  auto obj = Dict::getObject(data.value(), "obj");
  if (!obj) {
    L_ERROR("add sub data missing obj");
    return false;
  }
  
  string coll;
  if (!Storage::instance()->collName(type.value(), &coll, false)) {
    return false;
  }

  // clean up the object.
  DictO obj2;
  for (auto i: *obj) {
    if (get<0>(i) != "id" && get<0>(i) != "type") {
      obj2[get<0>(i)] = get<1>(i);
    }
  }
  
  // and insert it.
  auto result = SchemaImpl::insertGeneral(coll, obj2);
  if (!result) {
    L_ERROR("upd sub failed to update db");
    return false;
  }

  return true;
  
}

bool Server::shouldIgnoreAdd(const DictO &msg) {

  string mirror = get1Info("upstreamMirror");
  if (mirror == "true") {
    // always add when we are a mirror.
    return false;
  }
  
  auto data = Dict::getObject(msg, "data");
  if (!data) {
    return true;
  }
  
  auto type = Dict::getString(data.value(), "type");
  if (!type) {
    return true;
  }
  
  auto obj = Dict::getObject(data.value(), "obj");
  if (!obj) {
    return true;
  }

  if (!isObjParentUpstream(type.value(), obj.value())) {
    L_TRACE(type.value() << " parent upstream, ignoring add");
    return true;
  }

  return false;
  
}
