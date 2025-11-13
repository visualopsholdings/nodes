/*
  requestbuild.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 7-Feb-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "data.hpp"

namespace nodes {

void requestBuildMsg(Server *server, const IncomingMsg &in) {

  auto obj = Dict::getObject(rfl::to_generic(in));
  if (!obj) {
    server->sendErr("can't convert to object!");
    return;
  }
  server->setSrc(&(*obj));
  server->sendDataReq(nullopt, *obj);
    
  server->sendAck();
  
}

};