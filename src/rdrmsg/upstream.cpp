/*
  upstream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"

#include "log.hpp"

namespace nodes {

void upstreamMsg(Server *server, json &j) {
   
  auto msg = Json::getString(j, "msg", true);
  if (msg) {
    L_ERROR("online err " << msg.value());
    return;
  }
  
  auto type = Json::getString(j, "type");
  if (!type) {
    L_ERROR("reply missing type");
    return;
  }
  
  if (type.value() == "upstream") {
  
    auto id = Json::getString(j, "id");
    if (!id) {
      L_ERROR("got upstream with no id");
      return;
    }
    
    if (server->_upstreamId == id.value()) {
      L_WARNING("ignoring second upstream with the same id");
      return;
    }

    server->_upstreamId = id.value();
    server->_online = true;
    L_TRACE("upstream " << server->_upstreamId);
    
    auto valid = Json::getBool(j, "valid");
    if (!valid || !valid.value()) {
      L_INFO("we are not a valid node. Don't sync.");
      return;
    }
    
    server->sendUpDiscoverLocal(Json::getString(j, "corr", true));
    return;

  }
  L_ERROR("unknown msg type " << type.value());
     
}

};