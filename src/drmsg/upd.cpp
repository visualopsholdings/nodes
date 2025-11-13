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

void updDrMsg(Server *server, const IncomingMsg &in) {
   
  auto dest = Dict::getString(in.extra_fields.get("dest"));
  if (!dest) {
    server->sendErrDown("upd missing dest");
    return;
  }
  
  if (dest.value() != server->_serverId) {
   L_TRACE("not for us");
   server->sendAckDown();
   return;
  }
  
  auto obj = server->toObject(in);
  if (!obj) {
    server->sendErr("can't convert to object!");
    return;
  }

  server->updateObject(*obj);
  server->sendOn(*obj);
  server->sendAckDown();

  
}

};