/*
  searchusers.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void searchUsersMsg(Server *server, const IncomingMsg &in) {

  auto q = Dict::getString(in.extra_fields.get("q"));
  if (!q) {
    server->sendErr("no q");
    return;
  }

  auto docs = User().find(dictO({
    { "fullname", dictO({{ "$regex", q.value() }, { "$options", "i" }}) 
  }}), { "id", "modifyDate", "name", "fullname", "admin" }).all();

  DictV s;
  if (docs) {
    for (auto i: *docs) {
      s.push_back(i.dict());
    }
  }

  server->send(dictO({
    { "type", "searchusers" },
    { "result", s }
  }));
  
}

};