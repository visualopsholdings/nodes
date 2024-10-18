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

#include <boost/log/trivial.hpp>
#include <algorithm>

namespace nodes {

void discoverResultMsg(Server *server, json &j) {
   
  auto msgs = Json::getArray(j, "msgs");
  if (!msgs) {
    BOOST_LOG_TRIVIAL(error) << "discoverResult missing msgs";
    return;
  }
  
  // import everything.
  server->importObjs(msgs.value());
   
  server->setInfo("hasInitialSync", "true");
  json date = Storage::instance()->getNow();
  server->setInfo("upstreamLastSeen", Json::toISODate(date));
  server->systemStatus("Discovery complete");

  // make sure everything is regenerated
  Security::instance()->regenerateGroups();
  Security::instance()->regenerate();

}

};