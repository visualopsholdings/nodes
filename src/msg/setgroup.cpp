/*
  setgroup.cpp
  
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

void setGroupMsg(Server *server, json &j) {

  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id in group");
    return;
  }  
  
  Group groups;
  if (!Security::instance()->canEdit(groups, Json::getString(j, "me", true), id.value())) {
    BOOST_LOG_TRIVIAL(error) << "no edit for group " << id.value();
    server->sendSecurity();
    return;
  }

  auto doc = groups.findById(id.value(), {}).value();
  if (!doc) {
    server->sendErr("group not found");
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << "group old value " << doc.value().j();
  
  boost::json::object obj = {
    { "modifyDate", Storage::instance()->getNow() }
  };
  auto name = Json::getString(j, "name", true);
  if (name) {
    obj["name"] = name.value();
  }
  
  // send to other nodes.
  boost::json::object obj2 = obj;
  if (doc.value().upstream()) {
    obj2["upstream"] = true;
  }
  server->sendUpd("group", id.value(), obj2, "");
    
  // update locally
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