/*
  deletegroup.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "handler.hpp"
#include "log.hpp"

namespace nodes {

void deleteGroupMsg(Server *server, Data &j) {

  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no id");
    return;
  }

  Handler::remove(server, "group", id.value(), j.getString("me", true));

  Security::instance()->regenerateGroups();

}

};