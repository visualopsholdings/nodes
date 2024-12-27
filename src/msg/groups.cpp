/*
  groups.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "security.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void groupsMsg(Server *server, Data &j) {

  Group group;
  Data query = {
    { "deleted", {
      { "$ne", true }
      }
    }
  };
  L_TRACE(query);
  auto docs = Security::instance()->withView(group, j.getString("me", true), query, 
    { "id", "policy", "modifyDate", "name", "upstream" }).values();

  boost::json::array s;
  if (docs) {
    transform(docs.value().begin(), docs.value().end(), back_inserter(s), [](auto e) { return e.j(); });
  }

  server->sendCollection(j, "groups", s);
  
}

};