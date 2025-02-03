/*
  group.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void groupMsg(Server *server, Data &j) {

  auto groupid = j.getString("group");
  if (!groupid) {
    server->sendErr("no group");
    return;
  }

  Group group;
  auto doc = Security::instance()->withView(group, j.getString("me", true),  
    Data{ { "_id", { { "$oid", groupid.value() } } } }, 
    { "id", "name", "modifyDate" }).one();
  if (!doc) {
    L_ERROR("no group " + groupid.value());
    server->sendSecurity();
    return;
  }

  server->sendObject(j, "group", doc.value().d());
  
}

};