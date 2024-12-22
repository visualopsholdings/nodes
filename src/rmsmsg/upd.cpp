/*
  upd.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 15-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"
#include "storage/schema.hpp"

#include "log.hpp"

namespace nodes {

void updSubMsg(Server *server, json &j) {
   
  L_TRACE("upd or mov sub" << j);
        
  if (server->wasFromUs(j)) {
    L_TRACE("ignoring, came from us");
    return;
  }
  
  // keep sending it down.
  server->pubDown(j);

  // and write it ourselves
  server->updateObject(j);

}

};