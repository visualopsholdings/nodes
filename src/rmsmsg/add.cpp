/*
  add.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"
#include "storage/schema.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void addSubMsg(Server *server, json &j) {
   
  BOOST_LOG_TRIVIAL(trace) << "add sub" << j;
        
  auto src = Json::getString(j, "src");
  if (!src) {
    BOOST_LOG_TRIVIAL(error) << "upd sub missing src";
    return;
  }
  
  // keep sending it down.
  server->pubDown(j);

  // and write it ourselves
  server->addObject(j);

}

};