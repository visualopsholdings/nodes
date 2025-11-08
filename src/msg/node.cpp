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
#include "log.hpp"
#include "dict.hpp"

using namespace vops;

namespace nodes {

void nodeMsg(Server *server, const IncomingMsg &in) {

  auto node = Dict::getString(in.extra_fields.get("node"));
  if (!node) {
    server->sendErr("no node");
    return;
  }

  auto doc = Node().findById(*node).one();
  if (!doc) {
    L_ERROR("no nodes to view");
    server->sendSecurity();
    return;
  }

  server->sendObject(in, "node", doc.value().d().dict());

}

};