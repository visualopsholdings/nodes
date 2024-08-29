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
      if (!server->resetServer()) {
        server->sendErr("could not reset server");
      }
      server->sendAck();
      server->systemStatus("Server reset");
      return;
    }
    server->sendErr("can only set the serverId to none to reset it");
    return;
  }

  auto upstream = Json::getString(j, "upstream");
  if (upstream) {
    auto upstreamPubKey = Json::getString(j, "upstreamPubKey");
    if (!upstreamPubKey) {
      server->sendErr("missing upstreamPubKey");
      return;
    }
    if (upstream.value() == "none" && upstreamPubKey.value() == "none") {
      server->clearUpstream();
      server->sendAck();
      return;
    }
    if (!server->setInfo("upstream", upstream.value())) {
      server->sendErr("could not set upstream");
      return;
    }
    if (!server->setInfo("upstreamPubKey", upstreamPubKey.value())) {
      server->sendErr("could not set upstreamPubKey");
      return;
    }
    server->systemStatus("Upstream set");
    server->connectUpstream();
    server->sendAck();
    return;
  }
  
  stringstream ss;
  ss << "unknown " << j;
  server->sendErr(ss.str());

}

};