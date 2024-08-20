/*
  setinfo.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 8-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void setinfoMsg(Server *server, json &j) {

  auto serverId = Json::getString(j, "serverId");
  if (serverId) {
    if (serverId.value() == "none") {
      BOOST_LOG_TRIVIAL(trace) << "reset server";
      if (server->resetServer()) {
        server->sendAck();
        server->connectUpstream();
      }
      else {
        server->sendErr("could not reset server");
      }
      return;
    }
    return;
  }

  stringstream ss;
  ss << "unknown " << j;
  server->sendErr(ss.str());

}

};