/*
  stream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void streamMsg(Server *server, json &j) {

  auto streamid = Json::getString(j, "stream");
  if (!streamid) {
    server->sendErr("no stream");
    return;
  }

  auto doc = Stream().find(json{ { "_id", { { "$oid", streamid.value() } } } }).value();
  if (!doc) {
    server->sendErr("DB Error");
    return;
  }

  server->send({
    { "type", "stream" },
    { "stream", doc.value().j() }
  });

}

};