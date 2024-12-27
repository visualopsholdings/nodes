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

void setinfoMsg(Server *server, Data &j) {

  auto serverId = j.getString("serverId");
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

  auto upstream = j.getString("upstream");
  if (upstream) {
    auto upstreamPubKey = j.getString("upstreamPubKey");
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
    auto upstreamMirror = j.getString("upstreamMirror", true);
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