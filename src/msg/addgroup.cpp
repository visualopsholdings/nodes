/*
  addgroup.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"
#include "handler.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void addGroupMsg(Server *server, json &j) {

  auto upstream = Json::getBool(j, "upstream", true);
  if (upstream && upstream.value()) {
  
    auto id = Json::getString(j, "id");
    if (!id) {
      server->sendErr("no group id");
      return;
    }
  
    Handler::upstream(server, "group", id.value(), "name");
    return;
  }
  
  auto me = Json::getString(j, "me");
  if (!me) {
    server->sendErr("no me");
    return;
  }
  auto name = Json::getString(j, "name");
  if (!name) {
    server->sendErr("no name");
    return;
  }
  
  if (Handler::add(server, "group", me.value(), name.value())) {
    Security::instance()->regenerateGroups();
  }

}

};