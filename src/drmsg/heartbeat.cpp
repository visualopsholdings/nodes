/*
  heartbeat.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 3-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage/schema.hpp"
#include "date.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void heartbeatMsg(Server *server, Data &j) {
   
  L_TRACE("heartbeat " << j);
         
  string src;
  if (!server->getSrc(j, &src)) {
    server->sendErrDown("heartbeat missing src");
    return;
  }
  
  auto node = Node().find(dictO({{ "serverId", src }}), {}).one();
  if (!node) {
    L_ERROR("heartbeat no node " << src);
    server->sendAckDown();
    return;
  }
    
  auto now = Date::now();
  Node().updateById(node.value().id(), dictO({{ "lastSeen", dictO({{ "$date", now }}) }}));

  auto date = Date::toISODate(now);
  server->publish(nullopt, {
    { "type", "nodeSeen" },
    { "serverId", src },
    { "lastSeen", date }
  });
    
  server->sendDown(dictO({
    { "type", "date" },
    { "date", date },
  }));
  
}

};
