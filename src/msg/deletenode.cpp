/*
  deletenode.cpp
  
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

void deleteNodeMsg(Server *server, Data &j) {

  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no id");
    return;
  }

  if (Node().deleteById(id.value())) {
    server->sendAck();
    return;
  }

  server->sendErr("could not remove node");

}

};