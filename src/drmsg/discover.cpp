/*
  discover.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 3-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"
#include "storage/schema.hpp"
#include "date.hpp"
#include "log.hpp"

using namespace bsoncxx::builder::basic;

namespace nodes {

void discoverMsg(Server *server, json &j) {
   
  L_TRACE("discover " << j);
         
  server->sendDownDiscoverResult(j);
    
}

};
