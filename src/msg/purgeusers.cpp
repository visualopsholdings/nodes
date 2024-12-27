/*
  purgeusers.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 18-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "security.hpp"

#include "log.hpp"

namespace nodes {

void purgeUsersMsg(Server *server, Data &j) {

  User().deleteMany({ { "deleted", true } });
  server->sendAck();
  
}

};