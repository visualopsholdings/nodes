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
#include "log.hpp"

namespace nodes {

void infosMsg(Server *server, const IncomingMsg &in) {

  auto docs = Info().find({{}}, {"type", "text"}).all();

  // copy out all the data to return;
  DictV s;
  if (docs) {
    transform(docs->begin(), docs->end(), back_inserter(s), [](auto e) { return e.d().dict(); });
  }

  server->sendCollection(in, "infos", s);

}

};