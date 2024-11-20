/*
  purgeideas.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 20-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "security.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void purgeIdeasMsg(Server *server, json &j) {

  auto stream = Json::getString(j, "stream");
  if (!stream) {
    server->sendErr("no stream");
    return;
  }

  Idea().deleteMany({ { "stream", stream.value() }, { "deleted", true } });
  server->sendAck();
  
}

};