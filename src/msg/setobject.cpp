/*
  setobject.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "handler.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void setObjectMsg(Server *server, Data &j) {

  auto objtype = j.getString("objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }
  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no id in set object");
    return;
  }  
  
//  L_TRACE(j);
  
  auto name = j.getString("name", true);
  
  Data obj2 = j;

  // set on all fields passed in except name.
  boost::json::object obj = obj2.as_object();
  obj.erase("type");
  obj.erase("objtype");
  obj.erase("me");
  obj.erase("id");
  obj.erase("name");

  L_TRACE("setting name " << (name ? name.value() : "(not)"));
  L_TRACE("setting obj " << obj2);
  
  Handler::update(server, objtype.value(),  id.value(), 
    j.getString("me", true), name, obj2);

}

};