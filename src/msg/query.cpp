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

void Server::queryMsg(json &j) {

  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    sendErr("require objtype");
    return;
  }
  
  if (objtype.value() != "user") {
    sendErr("only user queries available");
    return;
  }
  
  auto email = Json::getString(j, "email");
  if (!email) {
    sendErr("require email for user query");
    return;
  }

  auto socketid = Json::getString(j, "socketid");
  if (!socketid) {
    sendErr("require socketid for user query");
    return;
  }

  if (!_online) {
    sendErr("not online with upstream");
    return;
  }
 
  json user = {
    { "email", email.value() }
  };
	sendTo(_dataReq->socket(), {
    { "type", "query" },
    { "user", user },
    { "socketid", socketid.value() },
    { "src", _serverId }
  }, "req query");
    
  sendAck();
  
}
