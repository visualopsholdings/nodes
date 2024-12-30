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

void deleteObjectMsg(Server *server, Data &j) {

  auto objtype = j.getString("objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }

  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no id");
    return;
  }

  Handler::remove(server, objtype.value(), id.value(), j.getString("me", true));

}

};