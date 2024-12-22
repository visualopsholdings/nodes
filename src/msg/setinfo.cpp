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

#include "log.hpp"

namespace nodes {

void setinfoMsg(Server *server, json &j) {

  auto serverId = Json::getString(j, "serverId");
  if (serverId) {
    if (serverId.value() == "none") {
      L_TRACE("reset server");
      if (server->resetServer()) {
        server->_reload = true;
      }
      else {
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
      server->_reload = true;
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
    auto upstreamMirror = Json::getString(j, "upstreamMirror", true);
    if (!server->setInfo("upstreamMirror", upstreamMirror ? upstreamMirror.value() : "false")) {
      server->sendErr("could not set upstreamMirror");
      return;
    }

    server->systemStatus("Upstream set");
    server->connectUpstream();
    server->_reload = true;
    server->sendAck();
    return;
  }
  
  stringstream ss;
  ss << "unknown " << j;
  server->sendErr(ss.str());

}

};