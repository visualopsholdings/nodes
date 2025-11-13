/*
  user.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 25-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "date.hpp"
#include "log.hpp"
#include "dict.hpp"

namespace nodes {

void userMsg(Server *server, const IncomingMsg &in) {

  auto user = Dict::getString(in.extra_fields.get("user"));
  if (!user) {
    server->sendErr("user not string");
    return;
  }

  auto doc = User().find(dictO({{ "_id", dictO({{ "$oid", *user }}) }}), { "id", "modifyDate", "name", "fullname", "admin", "hash", "active" }).one();

  if (!doc) {
    server->sendErr("no user " + *user);
    return;
  }
  
  server->sendObject(in, "user", doc->dict());
  
}

};