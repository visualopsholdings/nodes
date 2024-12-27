/*
  setgroup.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"
#include "handler.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void setGroupMsg(Server *server, Data &j) {

  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no id in group");
    return;
  }  
  
  Handler::update(server, "group", id.value(), 
    j.getString("me", true), j.getString("name", true), {{}});
  
}

};