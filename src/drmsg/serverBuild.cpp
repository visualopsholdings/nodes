/*
  serveBuild.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Nov-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "log.hpp"

namespace nodes {

void serverBuildMsg(Server *server, const IncomingMsg &in) {

  L_TRACE("we ignore build messages for now");   
  server->sendAckDown();

}

};