/*
  group.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"
#include "dict.hpp"

namespace nodes {

void groupMsg(Server *server, const IncomingMsg &in) {

  auto groupid = Dict::getString(in.extra_fields.get("group"));
  if (!groupid) {
    server->sendErr("no group");
    return;
  }

  Group group;
  auto doc = Security::instance()->withView(group, in.me,  
    dictO({{ 
      "_id", dictO({{ "$oid", *groupid }})
    }}), 
    { "id", "name", "modifyDate" }).one();
  if (!doc) {
    L_ERROR("no group " + *groupid);
    server->sendSecurity();
    return;
  }

  server->sendObject(in, "group", doc.value().dict());
  
}

};