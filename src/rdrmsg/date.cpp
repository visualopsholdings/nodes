/*
  date.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"

#include "log.hpp"

namespace nodes {

void dateMsg(Server *server, json &j) {
   
  L_TRACE("date " << j);
     
  if (!server->_online) {
    server->sendUpOnline();
  }
  
  auto date = Json::getString(j, "date");
  if (date) {
    server->publish(nullopt, {
      { "type", "nodeSeen" },
      { "serverId", server->_upstreamId },
      { "upstreamLastSeen", date.value() }
    });
  }

}

};