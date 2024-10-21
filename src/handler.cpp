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

template <typename RowType>
bool Handler<RowType>::add(Server *server, Schema<RowType> &schema, const string &type, const string &me, const string &name) {

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
  auto id = schema.insert(obj);
  if (!id) {
    server->sendErr("could not insert " + type);
    return false;
  }
  
  obj["id"] = id.value();
  obj["type"] = type;

  // send to other nodes.
  server->sendAdd(type, obj, "");
    
  // and reply back
  server->sendAck(id.value());
  
  return true;
  
}

template <typename RowType>
bool Handler<RowType>::update(Server *server, Schema<RowType> &schema, const string &type, const string &id, optional<string> me, optional<string> name, boost::json::object *obj) {

  if (!Security::instance()->canEdit(schema, me, id)) {
    BOOST_LOG_TRIVIAL(error) << "no edit for " << type << " " << id;
    server->sendSecurity();
    return false;
  }

  auto orig = schema.findById(id, {}).value();
  if (!orig) {
    server->sendErr(type + " not found");
    return false;
  }
  
  BOOST_LOG_TRIVIAL(trace) << type << " old value " << orig.value().j();
  
  (*obj)["modifyDate"] = Storage::instance()->getNow();
  if (name) {
    (*obj)["name"] = name.value();
  }
  
  // send to other nodes.
  boost::json::object obj2 = (*obj);
  if (orig.value().upstream()) {
    obj2["upstream"] = true;
  }
  server->sendUpd(type, id, obj2, "");
    
  // update locally
  BOOST_LOG_TRIVIAL(trace) << "updating " << (*obj);
  auto result = schema.updateById(id, (*obj));
  if (!result) {
    server->sendErr("could not update " + type);
    return false;
  }
  
  // and reply back
  BOOST_LOG_TRIVIAL(trace) << "updated " << result.value();
  server->sendAck();

  return true;
  
}

template <typename RowType>
bool Handler<RowType>::upstream(Server *server, Schema<RowType> &schema, const string &type, const string &id, const string &namefield) {

  auto doc = schema.findById(id).value();
  if (doc) {
    // set the upstream on doc.
    auto result = schema.updateById(id, { 
      { "modifyDate", Storage::instance()->getNow() },
      { "upstream", true } 
    });
    if (!result) {
      server->sendErr("could not update " + type);
      return false;
    }
    server->sendAck(result.value());
    return false;
  }
  
  // insert a new object
  auto result = schema.insert({
    { "_id", { { "$oid", id } } },
    { namefield, "Waiting discovery" },
    { "upstream", true },
    { "modifyDate", { { "$date", 0 } } }
  });
  if (!result) {
    server->sendErr("could not insert " + type);
    return false;
  }
  
  // and reply back
  server->sendAck(result.value());
  
  // run discovery.  
  server->sendUpDiscover();
  return true;

}

template class Handler<UserRow>;
template class Handler<StreamRow>;
template class Handler<GroupRow>;