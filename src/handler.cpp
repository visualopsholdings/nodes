/*
  handler.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Oct-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "handler.hpp"

#include "server.hpp"
#include "json.hpp"
#include "security.hpp"
#include "storage.hpp"

#include <boost/log/trivial.hpp>

bool Handler::add(Server *server, const string &type, const string &me, const string &name) {

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(type, &coll, false)) {
    server->sendErr("Could not get collection name for Handler::add");
    return false;
  }
  
  auto policy = Security::instance()->findPolicyForUser(me);
  if (!policy) {
    server->sendErr("could not get policy");
    return false;
  }
  
  boost::json::object obj = {
    { "name", name },
    { "policy", policy.value() },
    { "modifyDate", Storage::instance()->getNow() },
    { "active", true }
  };
  
  // insert a new object
  auto id = SchemaImpl::insertGeneral(coll, obj);
  if (!id) {
    server->sendErr("could not insert " + type);
    return false;
  }
  
  obj["id"] = id.value();
  obj["type"] = type;

  // send to other nodes.
  server->sendAdd(type, obj);
    
  // and reply back
  server->sendAck(id.value());
  
  return true;
  
}

bool Handler::update(Server *server, const string &type, const string &id, optional<string> me, optional<string> name, boost::json::object *obj) {

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(type, &coll, false)) {
    server->sendErr("Could not get collection name for Handler::update");
    return false;
  }
  
  if (!Security::instance()->canEdit(coll, me, id)) {
    BOOST_LOG_TRIVIAL(error) << "no edit for " << type << " " << id;
    server->sendSecurity();
    return false;
  }

  auto result = SchemaImpl::findByIdGeneral(coll, id, {});
  if (!result) {
    server->sendErr(type + " can't find");
    return false;
  }
  auto orig = result->value();
  if (!orig) {
    server->sendErr(type + " not found");
    return false;
  }
  
  BOOST_LOG_TRIVIAL(trace) << type << " old value " << orig.value();
  
  (*obj)["modifyDate"] = Storage::instance()->getNow();
  if (name) {
    (*obj)["name"] = name.value();
  }
  
  // send to other nodes.
  boost::json::object obj2 = (*obj);
  if (orig.value().as_object().if_contains("upstream")) {
    obj2["upstream"] = Json::getBool(orig.value(), "upstream").value();
  }
  server->sendUpd(type, id, obj2);
    
  // update locally
  BOOST_LOG_TRIVIAL(trace) << "updating " << (*obj);
  auto r = SchemaImpl::updateGeneralById(coll, id, {
    { "$set", (*obj) }
  });
  if (!r) {
    server->sendErr("could not update " + type);
    return false;
  }
  
  // and reply back
  BOOST_LOG_TRIVIAL(trace) << "updated " << r.value();
  server->sendAck();

  return true;
  
}

bool Handler::remove(Server *server, const string &type, const string &id, optional<string> me) {

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(type, &coll, false)) {
    server->sendErr("Could not get collection name for Handler::remove");
    return false;
  }
  
  if (!Security::instance()->canEdit(coll, me, id)) {
    BOOST_LOG_TRIVIAL(error) << "no edit for " << type << " " << id;
    server->sendSecurity();
    return false;
  }

  auto result = SchemaImpl::findByIdGeneral(coll, id, {});
  if (!result) {
    server->sendErr(type + " can't find");
    return false;
  }
  auto orig = result->value();
  if (!orig) {
    server->sendErr(type + " not found");
    return false;
  }
  
  BOOST_LOG_TRIVIAL(trace) << type << " old value " << orig.value();
  
  boost::json::object obj = {
    { "deleted", true },
    { "modifyDate", Storage::instance()->getNow() }
  };
  
  // add in any parent field.
  string parent;
  if (Storage::instance()->parentInfo(type, &parent)) {
    auto pid = Json::getString(orig.value(), parent);
    if (!pid) {
      server->sendErr("missing " + parent + " in " + id);
      return false;
    }
    obj[parent] = pid.value();
  }
  
  // send to other nodes.
  boost::json::object obj2 = obj;
  if (orig.value().as_object().if_contains("upstream")) {
    obj2["upstream"] = Json::getBool(orig.value(), "upstream").value();
  }
  server->sendUpd(type, id, obj2);
    
  // update locally
  BOOST_LOG_TRIVIAL(trace) << "updating " << obj;
  auto r = SchemaImpl::updateGeneralById(coll, id, {
    { "$set", obj }
  });
  if (!r) {
    server->sendErr("could not update " + type);
    return false;
  }
  
  // and reply back
  BOOST_LOG_TRIVIAL(trace) << "updated " << r.value();
  server->sendAck();

  return true;
  
}

bool Handler::upstream(Server *server, const string &type, const string &id, const string &namefield) {

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(type, &coll, false)) {
    server->sendErr("Could not get collection name for Handler::upstream");
    return false;
  }
  
  auto result = SchemaImpl::findByIdGeneral(coll, id, {});
  if (!result) {
    server->sendErr(type + " can't find");
    return false;
  }
  auto doc = result->value();
  if (doc) {
    // set the upstream on doc.
    auto result = SchemaImpl::updateGeneralById(coll, id, {
      { "$set", { 
        { "modifyDate", Storage::instance()->getNow() },
        { "upstream", true } 
      } }
    });
    if (!result) {
      server->sendErr("could not update " + type);
      return false;
    }
    server->sendAck(result.value());
    return false;
  }
  
  // insert a new object
  auto r = SchemaImpl::insertGeneral(coll, {
    { "_id", { { "$oid", id } } },
    { namefield, "Waiting discovery" },
    { "upstream", true },
    { "modifyDate", { { "$date", 0 } } }
  });
  if (!r) {
    server->sendErr("could not insert " + type);
    return false;
  }
  
  // and reply back
  server->sendAck(r.value());
  
  // run discovery.  
  server->sendUpDiscover();
  return true;

}
