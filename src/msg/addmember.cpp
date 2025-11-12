/*
  addmember.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void addMemberMsg(Server *server, const IncomingMsg &in) {

  // TBD: this should be very similar code to setgroup!
  
  auto groupid = Dict::getString(in.extra_fields.get("group"));
  if (!groupid) {
    server->sendErr("no group");
    return;
  }

  Group groups;
  if (!Security::instance()->canEdit(groups, in.me, groupid.value())) {
    server->sendErr("no edit for group " + groupid.value());
    return;
  }

  if (!in.id) {
    server->sendErr("no id");
    return;
  }

  if (groups.addMember(groupid.value(), in.id.value())) {
    Security::instance()->regenerateGroups();
    server->sendAck();
    return;
  }

  server->sendErr("could not add member");
  
}

};