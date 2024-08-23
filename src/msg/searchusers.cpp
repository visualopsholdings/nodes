/*
  searchusers.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void searchUsersMsg(Server *server, json &j) {

  auto q = Json::getString(j, "q");
  if (!q) {
    server->sendErr("no q");
    return;
  }

  auto docs = User().find(json{ { "fullname", { { "$regex", q.value() }, { "$options", "i" } } } }, { "id", "modifyDate", "name", "fullname", "admin" }).values();

  boost::json::array s;
  if (docs) {
    for (auto i: docs.value()) {
      s.push_back(i.j());
    }
  }

  server->send({
    { "type", "searchusers" },
    { "result", s }
  });
  
}

};