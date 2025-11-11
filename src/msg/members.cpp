/*
  members.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void membersMsg(Server *server, const IncomingMsg &in) {

  auto groupid = Dict::getString(in.extra_fields.get("group"));
  if (!groupid) {
    server->sendErr("no group");
    return;
  }

  Group groups;
  auto doc = Security::instance()->withView(groups, in.me, 
    dictO({{ "_id", dictO({{ "$oid", groupid.value() }}) }}), 
    { "members", "modifyDate" }).one();
  if (!doc) {
    L_ERROR("no group " + groupid.value());
    server->sendSecurity();
    return;
  }

  auto members = Dict::getVectorG(doc->dict(), "members");
  if (!members) {
    server->sendErr("no members in doc");
    return;
  }

  if (server->testModifyDate(in, doc->dict())) {
    server->send(dictO({
      { "type", "members" },
      { "test", dictO({
        { "latest", true }
        })
      }
    }));
    return;
  }
  
  DictV newmembers;
  for (auto i: *members) {
    auto o = Dict::getObject(i);
    if (!o) {
      L_ERROR("memer not an object");
      continue;
    }
    DictO m = *o;
    // set the fuillname.
    auto u = Dict::getString(m);
    if (u) {
      auto user = User().findById(*u, { "fullname" }).one();
      if (user) {
        m["fullname"] = user->fullname();
      }
    }
    newmembers.push_back(m);
  }
  
  server->send(dictO({
    { "type", "members" },
    { "group", dictO({
      { "members", newmembers },
      { "modifyDate", doc.value().modifyDate() }
      })
    }
  }));
  
}

};