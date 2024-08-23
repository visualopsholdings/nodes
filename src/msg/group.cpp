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

#include <boost/log/trivial.hpp>

namespace nodes {

void groupMsg(Server *server, json &j) {

  auto groupid = Json::getString(j, "group");
  if (!groupid) {
    server->sendErr("no group");
    return;
  }

  auto doc = Group().find(json{ { "_id", { { "$oid", groupid.value() } } } }, { "id", "name" }).value();

  if (!doc) {
    server->sendErr("can't find group " + groupid.value());
    return;
  }
  
  server->send({
    { "type", "group" },
    { "group", doc.value().j() }
  });
  
}

};