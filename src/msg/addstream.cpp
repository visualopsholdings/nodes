/*
  addstream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"
#include "handler.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void addStreamMsg(Server *server, json &j) {

  Stream streams;

  auto upstream = Json::getBool(j, "upstream", true);
  if (upstream && upstream.value()) {

    auto id = Json::getString(j, "id");
    if (!id) {
      server->sendErr("no stream id");
      return;
    }
  
    Handler<StreamRow>::upstream(server, streams, "stream", id.value(), "name");
    return;
  }

  auto me = Json::getString(j, "me");
  if (!me) {
    server->sendErr("no me");
    return;
  }
  auto name = Json::getString(j, "name");
  if (!name) {
    server->sendErr("no name");
    return;
  }
  
  Handler<StreamRow>::add(server, streams, "stream", me.value(), name.value());

}

};