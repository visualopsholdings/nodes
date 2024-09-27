/*
  infosmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 7-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void infosMsg(Server *server, json &j) {

  auto docs = Info().find(json{{}}, {"type", "text"}).values();

  boost::json::array s;
  if (docs) {
    transform(docs.value().begin(), docs.value().end(), back_inserter(s), [](auto e) { return e.j(); });
  }

  server->sendCollection(j, "infos", s);

}

};