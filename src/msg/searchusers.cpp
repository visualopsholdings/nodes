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

void searchUsersMsg(Server *server, Data &j) {

  auto q = j.getString("q");
  if (!q) {
    server->sendErr("no q");
    return;
  }

  auto docs = User().find({ { "fullname", { { "$regex", q.value() }, { "$options", "i" } } } }, { "id", "modifyDate", "name", "fullname", "admin" }).values();

  boost::json::array s;
  if (docs) {
    for (auto i: docs.value()) {
      s.push_back(i.d());
    }
  }

  server->send({
    { "type", "searchusers" },
    { "result", s }
  });
  
}

};