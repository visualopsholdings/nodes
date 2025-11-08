/*
  groups.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"
#include "data.hpp"
#include "dict.hpp"

using namespace vops;

namespace nodes {

void groupsMsg(Server *server, const IncomingMsg &in) {

  Group group;
  DictO query{makeDictO({
    { "deleted", makeDictO({
      { "$ne", true }
      })
    }
  })};
  auto docs = Security::instance()->withView(group, in.me, query, 
    { "id", "policy", "modifyDate", "name", "upstream" }).all();

  DictV s;
  if (docs) {
    transform(docs->begin(), docs->end(), back_inserter(s), [](auto e) { return e.d().dict(); });
  }

  server->sendCollection(in, "groups", s);
  
}

};