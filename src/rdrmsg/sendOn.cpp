/*
  sendOn.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void sendOnMsg(Server *server, const IncomingMsg &in) {
   
  // old servers (NodeJS) still use socket id
  auto corr = Dict::getString(in.extra_fields.get("socketid"));
  if (!corr) {
    corr = in.corr;
  }
  auto obj = server->toObject(in);
  if (!obj) {
    server->sendErr("can't convert to object!");
    return;
  }
  server->publish(corr, *obj);
  
}

};