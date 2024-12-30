/*
  setgroupolicy.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 11-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void setGroupPolicyMsg(Server *server, Data &j) {

  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no id in group");
    return;
  }  
  
  Group groups;
  if (!Security::instance()->canEdit(groups, j.getString("me", true), id.value())) {
    server->sendErr("no edit for groups " + id.value());
    return;
  }

  auto orig = groups.findById(id.value(), {}).value();
  if (!orig) {
    server->sendErr("group not found");
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
  
  auto policy = Security::instance()->modifyPolicy(orig.value().policy(), add, remove);
  if (!policy) {
    server->sendErr("could not modify policy");
  }
  if (policy.value() == orig.value().policy()) {
    L_INFO("policy didn't change for " << id.value());
    server->sendAck();
  }
  
  Data obj = {
    { "modifyDate", Storage::instance()->getNow() },
    { "policy", policy.value() }
  };

  // send to other nodes.
  Data obj2 = obj;
  if (orig.value().upstream()) {
    obj2.setBool("upstream", true);
  }
  server->sendUpd("group", id.value(), obj2);
    
  L_TRACE("updating " << obj);
  auto result = groups.updateById(id.value(), obj);
  if (!result) {
    server->sendErr("could not update group");
    return;
  }
  
  L_TRACE("updated " << result.value());
  server->sendAck(result.value());

}

};