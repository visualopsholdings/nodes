/*
  stream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void streamMsg(Server *server, json &j) {

  auto streamid = Json::getString(j, "stream");
  if (!streamid) {
    server->sendErr("no stream");
    return;
  }

  Stream stream;
  auto doc = Security::instance()->withView(stream, Json::getString(j, "me"), {{ { "_id", { { "$oid", streamid.value() } } } }}).value();
  if (!doc) {
    BOOST_LOG_TRIVIAL(error) << "no streams to view";
    server->sendSecurity();
    return;
  }

  server->send({
    { "type", "stream" },
    { "stream", doc.value().j() }
  });

}

};