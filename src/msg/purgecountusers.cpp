/*
  purgecountusers.cpp
  
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

void purgeCountUsersMsg(Server *server, json &j) {

  server->send({
    { "type", "count" },
    { "count", SchemaImpl::countGeneral("users", { { "deleted", true } }) }
  });
  
}

};