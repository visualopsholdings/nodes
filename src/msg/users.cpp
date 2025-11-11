/*
  users.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "log.hpp"
#include "data.hpp"
#include "dict.hpp"

namespace nodes {

void usersMsg(Server *server, const IncomingMsg &in) {

  DictO query{dictO({
    { "deleted", dictO({
      { "$ne", true }
      })
    }
  })};
  auto docs = User().find(query, { "id", "modifyDate", "name", "fullname", "admin", "active", "upstream" }).all();

  DictV s;
  if (docs) {
    transform(docs->begin(), docs->end(), back_inserter(s), [](auto e) { 
      return e.dict(); 
    });
  }

  server->sendCollection(in, "users", s);
  
}

};