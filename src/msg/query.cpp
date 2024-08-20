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
  
  if (objtype.value() != "user") {
    server->sendErr("only user queries available");
    return;
  }
  
  auto email = Json::getString(j, "email");
  if (!email) {
    server->sendErr("require email for user query");
    return;
  }

  auto socketid = Json::getString(j, "socketid");
  if (!socketid) {
    server->sendErr("require socketid for user query");
    return;
  }

  if (!server->_online) {
    server->sendErr("not online with upstream");
    return;
  }
 
  json user = {
    { "email", email.value() }
  };
	server->sendDataReq({
    { "type", "query" },
    { "user", user },
    { "socketid", socketid.value() },
    { "src", server->_serverId }
  });
    
  server->sendAck();
  
}

};