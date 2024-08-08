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
#include "json.hpp"

#include <boost/log/trivial.hpp>

void Server::infosMsg(json &j) {

  auto docs = Info().find(json{{}}, {"type", "text"}).values();

  boost::json::array s;
  for (auto i: docs.value()) {
    s.push_back(i.j());
  }
  send({
    { "type", "infos" },
    { "infos", s }
  });

}
