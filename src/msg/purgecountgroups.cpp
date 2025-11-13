/*
  purgecountgroups.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "log.hpp"

namespace nodes {

void purgeCountGroupsMsg(Server *server, const IncomingMsg &in) {

  server->send(dictO({
    { "type", "count" },
    { "count", SchemaImpl::countGeneral("groups", dictO({{ "deleted", true }})) }
  }));
  
}

};