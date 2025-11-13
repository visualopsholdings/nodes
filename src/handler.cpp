/*
  handler.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Oct-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "handler.hpp"

#include "server.hpp"
#include "security.hpp"
#include "storage.hpp"
#include "log.hpp"

namespace nodes {

bool Handler::add(Server *server, const string &type, const DictO &obj, optional<string> corr) {

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(type, &coll, false)) {
    server->sendErr("Could not get collection name for Handler::add");
    return false;
  }
  
  // insert a new object
  auto id = SchemaImpl::insertGeneral(coll, obj);
  if (!id) {
    server->sendErr("could not insert " + type);
    return false;
  }
  
  auto obj2 = obj;
  obj2["id"] = *id;
  obj2["type"] = type;

  // send to other nodes.
  server->sendAdd(type, obj2);
    
  // publish as well
  if (corr) {
    server->publish(corr.value(), obj2);
  }
  
  // and reply back
  server->sendAck(id.value());
  
  return true;
  
}

bool Handler::update(Server *server, const string &type, const string &id, optional<string> me, optional<string> name, const DictO &obj) {

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(type, &coll, false)) {
    server->sendErr("Could not get collection name for Handler::update");
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
  
  L_TRACE(type << " old value " << Dict::toString(orig.value()));
  
  // make sure it can be edited. If it has a parent we use that field.
  string editcoll;
  string editid;
  string parentfield;
  string parenttype;
  if (Storage::instance()->parentInfo(type, &parentfield, &parenttype)) {
    if (!Storage::instance()->collName(parenttype, &editcoll, false)) {
      server->sendErr("Could not get collection name for Handler::update (parent)");
      return false;
    }
    auto id = Dict::getString(orig.value(), parentfield);
    if (!id) {
      server->sendErr("coll field not found");
      return false;
    }
    editid = id.value();
  }
  else {
    editcoll = coll;
    editid = id;
  }

  if (!Security::instance()->canEdit(editcoll, me, editid)) {
    L_ERROR("no edit for " << type << " " << id);
    server->sendSecurity();
    return false;
  }
  
  auto obj1 = obj;
  obj1["modifyDate"] = Storage::instance()->getNow();
  if (name) {
    obj1["name"] = *name;
  }
  
  // send to other nodes.
  auto obj2 = obj1;
  auto upstream = Dict::getBool(orig.value(), "upstream");
  if (upstream) {
    obj2["upstream"] = *upstream;
  }
  server->sendUpd(type, id, obj2);
    
  // update locally
  L_TRACE("updating " << Dict::toString(obj1));
  auto r = SchemaImpl::updateGeneralById(coll, id, dictO({
    { "$set", obj1 }
  }));
  if (!r) {
    server->sendErr("could not update " + type);
    return false;
  }
  
  // and reply back
  L_TRACE("updated " << Dict::toString(r.value()));
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
    L_ERROR("no edit for " << type << " " << id);
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
  
  L_TRACE(type << " old value " << Dict::toString(orig.value()));
  
  auto obj = dictO({
    { "deleted", true },
    { "modifyDate", Storage::instance()->getNow() }
  });
  
  // add in any parent field.
  string parent;
  if (Storage::instance()->parentInfo(type, &parent)) {
    auto pid = Dict::getString(orig.value(), parent);
    if (!pid) {
      server->sendErr("missing " + parent + " in " + id);
      return false;
    }
    obj[parent] = *pid;
  }
  
  // send to other nodes.
  auto obj2 = obj;
  auto upstream = Dict::getBool(orig.value(), "upstream");
  if (upstream) {
    obj2["upstream"] = *upstream;
  }
  server->sendUpd(type, id, obj2);
    
  // update locally
  L_TRACE("updating " << Dict::toString(obj));
  auto r = SchemaImpl::updateGeneralById(coll, id, dictO({
    { "$set", obj }
  }));
  if (!r) {
    server->sendErr("could not update " + type);
    return false;
  }
  
  // and reply back
  L_TRACE("updated " << Dict::toString(r.value()));
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
    auto result = SchemaImpl::updateGeneralById(coll, id, dictO({
      { "$set", dictO({ 
        { "modifyDate", Storage::instance()->getNow() },
        { "upstream", true } 
      }) }
    }));
    if (!result) {
      server->sendErr("could not update " + type);
      return false;
    }
    server->sendAck(result.value());
    return false;
  }
  
  // insert a new object
  auto r = SchemaImpl::insertGeneral(coll, dictO({
    { "_id", dictO({ { "$oid", id } }) },
    { namefield, "Waiting discovery" },
    { "upstream", true },
    { "modifyDate", dictO({ { "$date", 0 } }) }
  }));
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

} // nodes
