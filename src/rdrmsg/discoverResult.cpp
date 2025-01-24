/*
  discoverResult.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"
#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

#include <algorithm>

namespace nodes {

void discoverResultMsg(Server *server, Data &j) {
   
  auto msgs = j.getData("msgs");
  if (!msgs) {
    L_ERROR("discoverResult missing msgs");
    return;
  }
  
  // import everything.
  server->importObjs(msgs.value());
   
  auto more = j.getBool("more", true);
  if (more && more.value()) {
    // go again.
    server->sendUpDiscover();
  }
  else {
    server->setInfo("hasInitialSync", "true");
    Data date = Storage::instance()->getNow();
    server->setInfo("upstreamLastSeen", Json::toISODate(date));
    server->systemStatus("Discovery complete");
  
    // make sure everything is regenerated
    Security::instance()->regenerateGroups();
    Security::instance()->regenerate();
  }
 
}

};