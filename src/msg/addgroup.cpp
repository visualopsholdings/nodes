/*
  addgroup.cpp
  
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
#include "data.hpp"

namespace nodes {

void addGroupMsg(Server *server, const IncomingMsg &in) {

  auto upstream = Dict::getBool(in.extra_fields.get("upstream"));
  if (upstream && upstream.value()) {
  
    if (!in.id) {
      server->sendErr("no group id");
      return;
    }
  
    Handler::upstream(server, "group", in.id.value(), "name");
    return;
  }
  
  if (!in.me) {
    server->sendErr("no me");
    return;
  }
  auto name = Dict::getString(in.extra_fields.get("name"));
  if (!name) {
    server->sendErr("no name");
    return;
  }
  
  auto policy = Security::instance()->findPolicyForUser(in.me.value());
  if (!policy) {
    server->sendErr("could not get policy");
    return;
  }

  auto obj = dictO({
    { "name", name.value() },
    { "policy", policy.value() },
    { "modifyDate", Storage::instance()->getNowO() },
    { "active", true }
  });

  if (Handler::add(server, "group", obj, in.corr)) {
    Security::instance()->regenerateGroups();
  }

}

};