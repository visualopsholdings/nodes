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

void requestBuildMsg(Server *server, Data &j) {

  boost::json::object msg = j.as_object();
  server->setSrc(&msg);
  server->sendDataReq(nullopt, msg);
    
  server->sendAck();
  
}

};