/*
  nodesmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 7-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void nodesMsg(Server *server, json &j) {

  Node node;
  auto docs = Security::instance()->withView(node, nullopt, {{}}, {}).values();
  
  boost::json::array s;
  if (docs) {
    transform(docs.value().begin(), docs.value().end(), back_inserter(s), [](auto e) { return e.j(); });
  }
  
  server->sendCollection(j, "nodes", s);

}

};