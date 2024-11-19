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
  json query = {
    { "stream", streamid.value() },
    { "deleted", {
      { "$ne", true }
      }
    }
  };
  auto docs = Security::instance()->withView(ideas, Json::getString(j, "me", true), query).values();

  boost::json::array s;
  if (docs) {
    transform(docs.value().begin(), docs.value().end(), back_inserter(s), [](auto e) { return e.j(); });
  }
  
  server->sendCollection(j, "ideas", s);

}

};