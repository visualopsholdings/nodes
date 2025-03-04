/*
  build.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 7-Feb-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

namespace nodes {

void buildMsg(Server *server, Data &j) {

  server->send({
    { "type", "build" },
    { "build", "2" }
  });
  
}

};