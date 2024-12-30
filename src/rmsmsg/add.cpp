/*
  add.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void addSubMsg(Server *server, Data &j) {
   
  L_TRACE("add sub" << j);
        
  if (server->wasFromUs(j)) {
    L_TRACE("ignoring, came from us");
    return;
  }
  
  // if we should ignore it, then do so.
  if (server->shouldIgnoreAdd(j)) {
    return;
  }

  // keep sending it down.
  server->pubDown(j);

  // and write it ourselves
  server->addObject(j);

}

};