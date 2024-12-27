/*
  site.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 14-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void siteMsg(Server *server, Data &j) {

  auto doc = Site().find(json{{}}, {}).value();

  if (doc) {
    server->sendObject(j, "site", doc.value().j());
    return;
  }
  
  server->send({
    { "type", "site" },
    { "site", json{{}} }
  });
  
}

};