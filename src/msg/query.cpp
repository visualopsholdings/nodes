/*
  query.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 15-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "upstream.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void queryMsg(Server *server, json &j) {

  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErr("require objtype");
    return;
  }
  
  if (!server->_online) {
    server->sendErr("not online with upstream");
    return;
  }
 
  if (objtype.value() == "user") {
    auto email = Json::getString(j, "email");
    if (!email) {
      server->sendErr("require email for user query");
      return;
    }
    json user = {
      { "email", email.value() }
    };
    server->sendDataReq(Json::getString(j, "corr", true), {
      { "type", "query" },
      { "user", user }
    });
      
    server->sendAck();
    return;
  }
  
  if (objtype.value() == "group") {
    auto name = Json::getString(j, "name");
    if (!name) {
      server->sendErr("require name for group query");
      return;
    }
    json group = {
      { "name", name.value() }
    };
    server->sendDataReq(Json::getString(j, "corr", true), {
      { "type", "query" },
      { "group", group }
    });
      
    server->sendAck();
    return;
  }
  
  server->sendErr("only user and group queries available");
  
}

};