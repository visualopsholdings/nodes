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

  auto userid = Json::getString(j, "user");
  if (!userid) {
    server->sendErr("no user");
    return;
  }

  Stream streams;
  auto docs = Security::instance()->withView(streams, userid.value(), {{}}, { "id", "name", "policy" });
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
    { "user", userid.value() },
    { "streams", s }
  });

}

};