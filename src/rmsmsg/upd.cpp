/*
  upd.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 15-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void updSubMsg(Server *server, const IncomingMsg &in) {
   
  if (server->wasFromUs(in)) {
    L_TRACE("ignoring, came from us");
    return;
  }
  
  auto obj = server->toObject(in);
  if (!obj) {
    server->sendErr("can't convert to object!");
    return;
  }

  // keep sending it down.
  server->pubDown(*obj);

  // and write it ourselves
  server->updateObject(*obj);

}

};