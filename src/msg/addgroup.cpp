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
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void addGroupMsg(Server *server, json &j) {

  auto upstream = Json::getBool(j, "upstream", true);
  if (upstream && upstream.value()) {

    auto groupid = Json::getString(j, "id");
    if (!groupid) {
      server->sendErr("no groupid");
      return;
    }
  
    auto doc = Group().findById(groupid.value()).value();
    if (doc) {
      // set the upstream on doc.
      auto result = Group().updateById(groupid.value(), { 
        { "modifyDate", Storage::instance()->getNow() },
        { "upstream", true } 
      });
      if (!result) {
        server->sendErr("could not update group");
        return;
      }
      server->sendAck();
      return;
    }
    
    // insert a new group
    auto result = Group().insert({
      { "_id", { { "$oid", groupid.value() } } },
      { "name", "Waiting discovery" },
      { "upstream", true },
      { "modifyDate", { { "$date", 0 } } }
    });
    if (!result) {
      server->sendErr("could not insert groupid");
      return;
    }
    
    server->sendAck();
    
    // run discovery.  
    server->discover();
    return;
  }
  
  auto me = Json::getString(j, "me");
  if (!me) {
    server->sendErr("no me");
    return;
  }
  auto name = Json::getString(j, "name");
  if (!name) {
    server->sendErr("no name");
    return;
  }
  
  auto policy = Security::instance()->findPolicyForUser(me.value());
  if (!policy) {
    server->sendErr("could not get policy");
    return;
  }
  
  // insert a new strean
  auto result = Group().insert({
    { "name", name.value() },
    { "policy", policy.value() },
    { "modifyDate", Storage::instance()->getNow() }
  });
  if (!result) {
    server->sendErr("could not insert group");
    return;
  }
  
  Security::instance()->regenerateGroups();

  server->sendAck();

}

};