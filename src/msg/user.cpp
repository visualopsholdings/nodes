/*
  user.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 25-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

void Server::userMsg(json &j) {

  auto userid = Json::getString(j, "user");
  if (!userid) {
    sendErr("no userid");
    return;
  }

  auto doc = User().find(json{ { "_id", { { "$oid", userid.value() } } } }).value();

  send({
    { "type", "user" },
    { "user", doc.value().j() }
  });
  
}
