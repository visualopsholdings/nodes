/*
  upd.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 15-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "storage/schema.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void updDrMsg(Server *server, Data &j) {
   
  L_TRACE("upd or mov (dr) " << j);
       
  auto dest = j.getString("dest");
  if (!dest) {
    server->sendErrDown("upd missing dest");
    return;
  }
  
  if (dest.value() != server->_serverId) {
   L_TRACE("not for us");
   server->sendAckDown();
   return;
  }
  
  server->updateObject(j);
  server->sendOn(j);
  server->sendAckDown();

  
}

};