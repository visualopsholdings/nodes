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

void addGroupMsg(Server *server, Data &j) {

  auto upstream = j.getBool("upstream", true);
  if (upstream && upstream.value()) {
  
    auto id = j.getString("id");
    if (!id) {
      server->sendErr("no group id");
      return;
    }
  
    Handler::upstream(server, "group", id.value(), "name");
    return;
  }
  
  auto me = j.getString("me");
  if (!me) {
    server->sendErr("no me");
    return;
  }
  auto name = j.getString("name");
  if (!name) {
    server->sendErr("no name");
    return;
  }
  
  auto policy = Security::instance()->findPolicyForUser(me.value());
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

  if (Handler::add(server, "group", obj, j.getString("corr", true))) {
    Security::instance()->regenerateGroups();
  }

}

};