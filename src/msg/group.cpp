/*
  group.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "security.hpp"

#include "log.hpp"

namespace nodes {

void groupMsg(Server *server, json &j) {

  auto groupid = Json::getString(j, "group");
  if (!groupid) {
    server->sendErr("no group");
    return;
  }

  Group group;
  auto doc = Security::instance()->withView(group, Json::getString(j, "me", true),  
    json{ { "_id", { { "$oid", groupid.value() } } } }, 
    { "id", "name", "modifyDate" }).value();
  if (!doc) {
    L_ERROR("no group " + groupid.value());
    server->sendSecurity();
    return;
  }

  server->sendObject(j, "group", doc.value().j());
  
}

};