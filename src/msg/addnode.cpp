/*
  addnode.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 7-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void addNodeMsg(Server *server, const IncomingMsg &in) {

  auto serverId = Dict::getString(in.extra_fields.get("serverId"));
  if (!serverId) {
    server->sendErr("no serverId");
    return;
  }
  
  auto pubKey = Dict::getString(in.extra_fields.get("pubKey"));
  if (!pubKey) {
    server->sendErr("no pubKey");
    return;
  }
  
  // insert a new node
  auto result = Node().insert(dictO({
    { "serverId", serverId.value() },
    { "pubKey", pubKey.value() }
  }));
  if (!result) {
    server->sendErr("could not insert node");
    return;
  }
  
  server->sendAck(result.value());

}

};