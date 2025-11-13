/*
  discoverResult.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void discoverResultMsg(Server *server, const IncomingMsg &in) {
   
  auto msgs = Dict::getVector(in.extra_fields.get("msgs"));
  if (!msgs) {
    L_ERROR("discoverResult missing msgs");
    return;
  }
  
  // import everything.
  server->importObjs(msgs.value());
   
  server->setInfo("hasInitialSync", "true");
  auto date = Storage::instance()->getNow();
  server->setInfo("upstreamLastSeen", CollectionImpl::toISODate(date));

  auto more = Dict::getBool(in.extra_fields.get("more"));
  if (more && more.value()) {
    // go again.
    server->sendUpDiscover();
  }
  else {
    server->discoveryComplete();
  }
 
}

};