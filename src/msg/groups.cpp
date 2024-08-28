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

#include <boost/log/trivial.hpp>

namespace nodes {

void groupsMsg(Server *server, json &j) {

  Group group;
  auto docs = Security::instance()->withView(group, Json::getString(j, "me"), json{{}}, 
    { "id", "modifyDate", "name" }).values();

  boost::json::array s;
  for (auto i: docs.value()) {
    s.push_back(i.j());
  }
  server->send({
    { "type", "groups" },
    { "groups", s }
  });
  
}

};