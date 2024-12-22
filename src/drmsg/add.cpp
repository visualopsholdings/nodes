/*
  add.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"
#include "storage/schema.hpp"
#include "log.hpp"

namespace nodes {

void addDrMsg(Server *server, json &j) {
   
  L_TRACE("add (dr) " << j);
       
  auto dest = Json::getString(j, "dest");
  if (!dest) {
    server->sendErrDown("add missing dest");
    return;
  }
  
  if (dest.value() != server->_serverId) {
   L_TRACE("not for us");
   server->sendAckDown();
   return;
  }
  
  server->addObject(j);
  server->sendOn(j);
  server->sendAckDown();

  
}

};