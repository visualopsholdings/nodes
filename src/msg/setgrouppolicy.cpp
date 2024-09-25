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
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void setGroupPolicyMsg(Server *server, json &j) {

  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id in group");
    return;
  }  
  
  Group groups;
  if (!Security::instance()->canEdit(groups, Json::getString(j, "me", true), id.value())) {
    server->sendErr("can't edit groups " + id.value());
    return;
  }

  auto doc = groups.findById(id.value(), {}).value();
  if (!doc) {
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
  
  auto policy = Security::instance()->modifyPolicy(doc.value().policy(), add, remove);
  if (!policy) {
    server->sendErr("could not modify policy");
  }
  if (policy.value() == doc.value().policy()) {
    BOOST_LOG_TRIVIAL(info) << "policy didn't change for " << id.value();
    server->sendAck();
  }
  
  boost::json::object obj;
  obj["policy"] = policy.value();
  BOOST_LOG_TRIVIAL(trace) << "updating " << obj;
  auto result = groups.updateById(id.value(), obj);
  if (!result) {
    server->sendErr("could not update group");
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << "updated " << result.value();
  server->sendAck();

}

};