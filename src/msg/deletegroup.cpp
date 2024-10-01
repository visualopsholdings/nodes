/*
  deletegroup.cpp
  
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

void deleteGroupMsg(Server *server, json &j) {

  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id");
    return;
  }

  Group groups;
  auto doc = Security::instance()->withEdit(groups, Json::getString(j, "me", true),  
    json{ { "_id", { { "$oid", id.value() } } } }, 
    { "members" }).value();
  if (!doc) {
    server->sendErr("can't edit group " + id.value());
    return;
  }

  if (doc.value().members().size() > 0) {
    server->sendWarning("group is not empty");
    return;
  }
  
  if (!groups.deleteById(id.value())) {
    server->sendErr("could not remove group");
    return;
  }

  Security::instance()->regenerateGroups();
  server->sendAck();

}

};