/*
  deleteuser.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"
#include "handler.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void deleteUserMsg(Server *server, json &j) {

  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id");
    return;
  }

  User users;
  Handler<UserRow>::remove(server, users, "user", id.value(), 
    Json::getString(j, "me", true), false);

}

};