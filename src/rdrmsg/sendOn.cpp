/*
  sendOn.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "json.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void sendOnMsg(Server *server, Data &j) {
   
  // old servers (NodeJS) still use socket id
  auto corr = j.getString("socketid", true);
  if (!corr) {
    corr = j.getString("corr", true);
  }
  server->publish(corr, j);
  
}

};