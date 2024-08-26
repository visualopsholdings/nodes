/*
  deletemember.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void deleteMemberMsg(Server *server, json &j) {

  auto groupid = Json::getString(j, "group");
  if (!groupid) {
    server->sendErr("no group");
    return;
  }

  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id");
    return;
  }

  if (Group().removeMember(groupid.value(), id.value())) {
    server->sendAck();
    return;
  }

  server->sendErr("could not remover member");

}

};