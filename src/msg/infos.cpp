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
#include "log.hpp"

namespace nodes {

void infosMsg(Server *server, Data &j) {

  auto docs = Info().find({{}}, {"type", "text"}).values();

  boost::json::array s;
  if (docs) {
    transform(docs.value().begin(), docs.value().end(), back_inserter(s), [](auto e) { return e.d(); });
  }

  server->sendCollection(j, "infos", s);

}

};