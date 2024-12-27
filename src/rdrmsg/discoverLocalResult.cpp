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
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void discoverLocalResultMsg(Server *server, Data &) {
   
  // the server has inserted all the local stuff, discover what's out there.
  server->sendUpDiscover();

}

};