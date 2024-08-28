/*
  streamsmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void streamsMsg(Server *server, json &j) {

  Stream stream;
  auto docs = Security::instance()->withView(stream, Json::getString(j, "me"), {{}}, { "id", "name", "policy" }).values();
  if (!docs) {
    server->sendErr("DB Error");
    return;
  }
//  BOOST_LOG_TRIVIAL(trace) << docs.value();
  boost::json::array s;
  for (auto i: docs.value()) {
    s.push_back(i.j());
  }
  server->send({
    { "type", "streams" },
    { "streams", s }
  });

}

};