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
#include "log.hpp"

namespace nodes {

void canRegisterMsg(Server *server, Data &j) {

  auto token = j.getString("token");
  if (!token) {
    server->sendErr("Missing token");
    return;
  }
  
  auto exptoken = Security::instance()->expandShareToken(token.value());
  if (!exptoken) {
    server->sendErr("Could not expand token");
    return;
  }

  auto id = Dict::getString(exptoken.value(), "id");
  auto options = Dict::getString(exptoken.value(), "options");
  
  server->send(dictO({
    { "type", "canreg" },
    { "canRegister", id && options && options.value() != "none" },
    { "requireFullname", options && options.value() == "mustName" }
  }));

}

};