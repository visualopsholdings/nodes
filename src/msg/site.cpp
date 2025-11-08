/*
  site.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 14-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "log.hpp"
#include "data.hpp"
#include "dict.hpp"

using namespace vops;

namespace nodes {

void siteMsg(Server *server, const IncomingMsg &in) {

  auto doc = Site().find({{}}, {}).one();

  if (doc) {
    server->sendObject(in, "site", doc.value().d().dict());
    return;
  }
  
  server->send(makeDictO({
    { "type", "site" },
    { "site", makeDictO({}) }
  }));
  
}

};