/*
  purgestreams.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "security.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void purgeStreamsMsg(Server *server, json &j) {

  Stream().deleteMany({ { "deleted", true } });
  server->sendAck();
  
}

};