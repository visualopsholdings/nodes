/*
  users.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"

#include <boost/log/trivial.hpp>

void Server::usersMsg(json &j) {

  auto docs = User().find(json{{}}, { "id", "modifyDate", "name", "fullname"}).values();

  boost::json::array s;
  for (auto i: docs.value()) {
    s.push_back(i.j());
  }
  send({
    { "type", "users" },
    { "users", s }
  });
  
}
