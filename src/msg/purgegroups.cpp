/*
  purgegroups.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "security.hpp"

#include "log.hpp"

namespace nodes {

void purgeGroupsMsg(Server *server, json &j) {

  Group().deleteMany({ { "deleted", true } });
  Security::instance()->regenerateGroups();
  server->sendAck();
  
}

};