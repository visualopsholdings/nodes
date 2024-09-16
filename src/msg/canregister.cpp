/*
  canreg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void canRegisterMsg(Server *server, json &j) {

  server->send({
    { "type", "canreg" },
    { "canRegister", true },
    { "requireFullname", true }
  });

}

};