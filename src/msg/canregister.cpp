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

  auto token = Json::getString(j, "token");
  if (!token) {
    server->sendErr("Missing token");
    return;
  }
  
  auto exptoken = Security::instance()->expandShareToken(token.value());
  if (!exptoken) {
    server->sendErr("Could not expand token");
    return;
  }

  auto id = Json::getString(exptoken.value(), "id");
  auto options = Json::getString(exptoken.value(), "options");
  
  server->send({
    { "type", "canreg" },
    { "canRegister", id && options && options.value() != "none" },
    { "requireFullname", options && options.value() == "mustName" }
  });

}

};