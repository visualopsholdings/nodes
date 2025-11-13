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

namespace nodes {

void heartbeatMsg(Server *server, const IncomingMsg &in) {
   
  string src;
  if (!server->getSrc(in, &src)) {
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
  server->publish(nullopt, dictO({
    { "type", "nodeSeen" },
    { "serverId", src },
    { "lastSeen", date }
  }));
    
  server->sendDown(dictO({
    { "type", "date" },
    { "date", date },
  }));
  
}

};
