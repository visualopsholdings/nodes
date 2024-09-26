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

#include <boost/log/trivial.hpp>

namespace nodes {

void siteMsg(Server *server, json &j) {

  auto doc = Site().find(json{{}}, {}).value();

  if (doc) {
    if (server->testModifyDate(j, doc.value().j())) {
      server->send({
        { "type", "site" },
        { "test", {
          { "latest", true }
          }
        }
      });
      return;
    }
  }
  
  server->send({
    { "type", "site" },
    { "site", doc ? doc.value().j() : json{{}} }
  });
  
}

};