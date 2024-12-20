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
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void setObjectPolicyMsg(Server *server, json &j) {

  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }
  auto id = Json::getString(j, "id");
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
  
  if (!Security::instance()->canEdit(coll, Json::getString(j, "me", true), id.value())) {
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
  
  auto policyid = Json::getString(orig.value(), "policy");
  if (!policyid) {
    server->sendErr(objtype.value() + " no policy id");
    return;
  }
  
  auto policy = Security::instance()->modifyPolicy(policyid.value(), add, remove);
  if (!policy) {
    server->sendErr("could not modify policy");
  }
  if (policy.value() == policyid.value()) {
    BOOST_LOG_TRIVIAL(info) << "policy didn't change for " << id.value();
    server->sendAck();
  }
  
  boost::json::object obj = {
    { "modifyDate", Storage::instance()->getNow() },
    { "policy", policy.value() }
  };

  // send to other nodes.
  boost::json::object obj2 = obj;
  if (orig.value().as_object().if_contains("upstream")) {
    obj2["upstream"] = Json::getBool(orig.value(), "upstream").value();
  }
  server->sendUpd(objtype.value(), id.value(), obj2);
    
  BOOST_LOG_TRIVIAL(trace) << "updating " << obj;
  auto r = SchemaImpl::updateGeneralById(coll, id.value(), {
    { "$set", obj }
  });
  
  BOOST_LOG_TRIVIAL(trace) << "updated " << r.value();
  server->sendAck(r.value());

}

};