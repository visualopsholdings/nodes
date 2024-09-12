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
  if (!Security::instance()->canEdit(groups, Json::getString(j, "me"), id.value())) {
    server->sendErr("can't edit groups " + id.value());
    return;
  }

  auto doc = groups.findById(id.value(), {}).value();
  if (!doc) {
    server->sendErr("group not found");
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << "group old value " << doc.value().j();
  
  boost::json::object obj;
  auto name = Json::getString(j, "name");
  if (name) {
    obj["name"] = name.value();
  }
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