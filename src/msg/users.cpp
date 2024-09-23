/*
  users.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void usersMsg(Server *server, json &j) {

  auto docs = User().find(json{{}}, { "id", "modifyDate", "name", "fullname", "admin", "active", "upstream" }).values();

  boost::json::array s;
  if (docs) {
    transform(docs.value().begin(), docs.value().end(), back_inserter(s), [](auto e) { return e.j(); });
  }

  server->send({
    { "type", "users" },
    { "users", s }
  });
  
}

};