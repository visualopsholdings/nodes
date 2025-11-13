/*
  deleteobject.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "handler.hpp"

#include "log.hpp"

namespace nodes {

void deleteObjectMsg(Server *server, const IncomingMsg &in) {

  if (!in.objtype) {
    server->sendErr("no object type");
    return;
  }

  if (!in.id) {
    server->sendErr("no id");
    return;
  }

  Handler::remove(server, in.objtype.value(), in.id.value(), in.me);

}

};