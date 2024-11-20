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

#include <boost/log/trivial.hpp>

namespace nodes {

void updSubMsg(Server *server, json &j) {
   
  BOOST_LOG_TRIVIAL(trace) << "upd sub" << j;
        
  auto src = Json::getString(j, "src");
  if (!src) {
    BOOST_LOG_TRIVIAL(error) << "upd sub missing src";
    return;
  }
  if (src.value() == server->_serverId) {
    BOOST_LOG_TRIVIAL(trace) << "ignoring, came from us";
    return;
  }
  
  // keep sending it down.
  server->pubDown(j);

  // and write it ourselves
  server->updateObject(j);

}

};