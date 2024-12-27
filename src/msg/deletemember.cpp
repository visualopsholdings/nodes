/*
  deletemember.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include "log.hpp"

namespace nodes {

void deleteMemberMsg(Server *server, Data &j) {

  auto groupid = j.getString("group");
  if (!groupid) {
    server->sendErr("no group");
    return;
  }

  Group groups;
  if (!Security::instance()->canEdit(groups, j.getString("me", true), groupid.value())) {
    server->sendErr("no edit for group " + groupid.value());
    return;
  }

  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no id");
    return;
  }

  if (groups.removeMember(groupid.value(), id.value())) {
    Security::instance()->regenerateGroups();
    server->sendAck();
    return;
  }

  server->sendErr("could not remove member");

}

};