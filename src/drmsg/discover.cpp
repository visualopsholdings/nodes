/*
  discover.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 3-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "log.hpp"
#include "data.hpp"

using namespace bsoncxx::builder::basic;

namespace nodes {

void discoverMsg(Server *server, Data &data) {
   
  L_TRACE("discover " << data);
         
  server->sendDownDiscoverResult(data);
    
}

};
