/*
  add.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void addDrMsg(Server *server, Data &data) {
   
  L_TRACE("add (dr) " << data);
       
  auto dest = data.getString("dest");
  if (!dest) {
    server->sendErrDown("add missing dest");
    return;
  }
  
  if (dest.value() != server->_serverId) {
   L_TRACE("not for us");
   server->sendAckDown();
   return;
  }
  
  server->addObject(data);
  server->sendOn(data);
  server->sendAckDown();

  
}

};