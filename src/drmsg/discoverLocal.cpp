/*
  discoverLocal.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 3-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void discoverLocalMsg(Server *server, Data &j) {
   
  L_TRACE("discoverLocal " << j);
 
  auto data = j.getData("data");
  if (!data) {
    server->sendErrDown("discoverLocal missing data");
    return;
  }
  
  // import everything.
  server->importObjs(data.value());
 	    
 	server->sendDown({ 
 	  { "type", "discoverLocalResult" } 
 	});
 	
}

};
