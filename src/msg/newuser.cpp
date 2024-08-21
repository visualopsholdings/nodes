/*
  newuser.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 20-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void newUserMsg(Server *server, json &j) {

  auto userid = Json::getString(j, "id");
  if (!userid) {
    server->sendErr("no userid");
    return;
  }

  auto upstream = Json::getBool(j, "upstream");
  if (!upstream || !upstream.value()) {
    server->sendErr("only upstream for now.");
    return;
  }

  auto doc = User().find(json{ { "_id", { { "$oid", userid.value() } } } }, {}).value();
  if (doc) {
    // set the upstream on doc.
    auto result = User().updateById(userid.value(), {{ "$set", {{ "upstream", true }} }});
    if (!result) {
      server->sendErr("could not update user");
      return;
    }
    server->sendAck();
    return;
  }
  
  // insert a new user
  auto result = User().insert({
    { "_id", { { "$oid", userid.value() } } },
    { "fullname", "Waiting discovery" },
    { "upstream", true },
    { "modifyDate", { { "$date", 0 } } }
  });
  if (!result) {
    server->sendErr("could not insert user");
    return;
  }
  
  server->sendAck();
  
}

};