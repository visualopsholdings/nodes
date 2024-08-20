/*
  sendOn.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void sendOnMsg(Server *server, json &j) {
   
  server->publish(j);
  
}

};