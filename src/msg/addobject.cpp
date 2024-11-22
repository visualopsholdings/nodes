/*
  addobject.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Nov-2024
  
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

void addObjectMsg(Server *server, json &j) {

  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }

  auto upstream = Json::getBool(j, "upstream", true);
  if (upstream && upstream.value()) {

    auto id = Json::getString(j, "id");
    if (!id) {
      server->sendErr("no id");
      return;
    }
  
    Handler::upstream(server, objtype.value(), id.value(), "name");
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
  
  Handler::add(server, objtype.value(), me.value(), name.value());

}

};