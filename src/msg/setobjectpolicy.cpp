/*
  setobjectpolicy.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void setObjectPolicyMsg(Server *server, Data &j) {

  auto objtype = j.getString("objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }
  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no id in " + objtype.value());
    return;
  }  
  
  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(objtype.value(), &coll)) {
    server->sendErr("Could not get collection name for setobjectpolicy");
    return;
  }
  
  if (!Security::instance()->canEdit(coll, j.getString("me", true), id.value())) {
    server->sendErr("no edit for " + objtype.value() + " " + id.value());
    return;
  }

  auto result = SchemaImpl::findByIdGeneral(coll, id.value(), {});
  if (!result) {
    server->sendErr(objtype.value() + " can't find");
    return;
  }
  auto orig = result->value();
  if (!orig) {
    server->sendErr(objtype.value() + " not found");
    return;
  }

  // build our add and remove vectors.
  vector<addTupleType> add;
  auto arr = j.at("add").as_array();
  transform(arr.begin(), arr.end(), back_inserter(add), [](auto e) { 
    return addTupleType { e.at("type").as_string().c_str(), e.at("context").as_string().c_str(), e.at("_id").as_string().c_str() }; 
  });
  
  vector<string> remove;
  arr = j.at("remove").as_array();
  transform(arr.begin(), arr.end(), back_inserter(remove), [](auto e) { return e.as_string().c_str(); });
  
  auto policyid = Dict::getString(orig.value(), "policy");
  if (!policyid) {
    server->sendErr(objtype.value() + " no policy id");
    return;
  }
  
  auto policy = Security::instance()->modifyPolicy(policyid.value(), add, remove);
  if (!policy) {
    server->sendErr("could not modify policy");
  }
  if (policy.value() == policyid.value()) {
    L_INFO("policy didn't change for " << id.value());
    server->sendAck();
  }
  
  auto obj = dictO({
    { "modifyDate", Storage::instance()->getNowO() },
    { "policy", policy.value() }
  });

  // send to other nodes.
  auto obj2 = obj;
  auto upstream = Dict::getBool(orig.value(), "upstream");
  if (upstream) {
    obj2["upstream"] = *upstream;
  }
  server->sendUpd(objtype.value(), id.value(), obj2);
    
  L_TRACE("updating " << Dict::toString(obj));
  auto r = SchemaImpl::updateGeneralById(coll, id.value(), dictO({
    { "$set", obj }
  }));
  
  L_TRACE("updated " << Dict::toString(r.value()));
  server->sendAck(r.value());

}

};