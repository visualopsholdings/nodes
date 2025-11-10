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
#include "log.hpp"
#include "dict.hpp"

namespace nodes {

void nodesMsg(Server *server, const IncomingMsg &in) {

  auto docs = Node().find(dictO({})).all();
  
  DictV s;
  if (docs) {
    transform(docs->begin(), docs->end(), back_inserter(s), [](auto e) { return e.d().dict(); });
  }
  
  server->sendCollection(in, "nodes", s);

}

};