/*
  node.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 7-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include "log.hpp"

namespace nodes {

void nodeMsg(Server *server, json &j) {

  auto nodeid = Json::getString(j, "node");
  if (!nodeid) {
    server->sendErr("no node");
    return;
  }

  auto doc = Node().findById(nodeid.value()).value();
  if (!doc) {
    L_ERROR("no nodes to view");
    server->sendSecurity();
    return;
  }

  server->sendObject(j, "node", doc.value().j());

}

};