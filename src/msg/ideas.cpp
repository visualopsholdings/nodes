/*
  ideas.cpp
  
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

void ideasMsg(Server *server, json &j) {

  auto streamid = Json::getString(j, "stream");
  if (!streamid) {
    server->sendErr("no stream");
    return;
  }

  Idea ideas;
  auto docs = Security::instance()->withView(ideas, Json::getString(j, "me", true), 
    { { "stream", streamid.value() } }).values();

  boost::json::array s;
  if (docs) {
    transform(docs.value().begin(), docs.value().end(), back_inserter(s), [](auto e) { return e.j(); });
  }
  
  server->sendCollection(j, "ideas", s);

//   auto docs = Idea().find(json{ { "stream", streamid.value() } }).values();
//   if (!docs) {
//     boost::json::array empty;
//     server->send({
//       { "type", "ideas" },
//       { "stream", streamid.value() },
//       { "ideas", empty }
//     });
//     return;
//   }
// 
//   boost::json::array s;
//   for (auto i: docs.value()) {
//     s.push_back(i.j());
//   }
// 
//   server->send({
//     { "type", "ideas" },
//     { "stream", streamid.value() },
//     { "ideas", s }
//   });
// 
}

};