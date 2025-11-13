/*
  upstream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "log.hpp"

namespace nodes {

void upstreamMsg(Server *server, const IncomingMsg &in) {
   
  auto msg = Dict::getString(in.extra_fields.get("msg"));
  if (msg) {
    L_ERROR("online err " << msg.value());
    return;
  }
  
  if (in.type == "upstream") {
  
    if (!in.id) {
      L_ERROR("got upstream with no id");
      return;
    }
    
    if (server->_upstreamId == in.id.value()) {
      L_WARNING("ignoring second upstream with the same id");
      return;
    }

    server->_upstreamId = in.id.value();
    server->_online = true;
    L_TRACE("upstream " << server->_upstreamId);
    
    auto valid = Dict::getBool(in.extra_fields.get("valid"));
    if (!valid || !valid.value()) {
      L_INFO("we are not a valid node. Don't sync.");
      return;
    }
    
    server->sendUpDiscoverLocal(in.corr);
    return;

  }
  L_ERROR("unknown msg type " << in.type);
     
}

};