/*
  user.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 25-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "date.hpp"

#include "log.hpp"

namespace nodes {

void userMsg(Server *server, json &j) {

  auto userid = Json::getString(j, "user");
  if (!userid) {
    server->sendErr("no userid");
    return;
  }

  auto doc = User().find(json{ { "_id", { { "$oid", userid.value() } } } }, { "id", "modifyDate", "name", "fullname", "admin", "hash", "active" }).value();

  if (!doc) {
    server->sendErr("no user " + userid.value());
    return;
  }
  
  server->sendObject(j, "user", doc.value().j());
  
}

};