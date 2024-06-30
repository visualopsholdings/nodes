/*
  streamsmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"

#include <boost/log/trivial.hpp>

void Server::streamsMsg(json &j) {

  string userid;
  if (!getString(j, "user", &userid)) {
    sendErr("no user");
    return;
  }

  Stream streams;
  auto docs = Security::instance()->withView(streams, userid, {{}}, { "id", "name", "policy" });
  if (!docs) {
    sendErr("DB Error");
    return;
  }
//  BOOST_LOG_TRIVIAL(trace) << docs.value();
  boost::json::array s;
  for (auto i: docs.value()) {
    s.push_back(i.j());
  }
  send({
    { "type", "streams" },
    { "user", userid },
    { "streams", s }
  });

}
