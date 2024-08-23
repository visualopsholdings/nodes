/*
  discoverLocalResult.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage/schema.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void discoverLocalResultMsg(Server *server, json &) {
   
  // the server has inserted all the local stuff, discover what's out there.
  server->discover();

}

};