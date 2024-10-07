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
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void addNodeMsg(Server *server, json &j) {

  auto serverId = Json::getString(j, "serverId");
  if (!serverId) {
    server->sendErr("no serverId");
    return;
  }
  
  auto pubKey = Json::getString(j, "pubKey");
  if (!pubKey) {
    server->sendErr("no pubKey");
    return;
  }
  
  // insert a new node
  auto result = Node().insert({
    { "serverId", serverId.value() },
    { "pubKey", pubKey.value() }
  });
  if (!result) {
    server->sendErr("could not insert node");
    return;
  }
  
  server->sendAck();

}

};