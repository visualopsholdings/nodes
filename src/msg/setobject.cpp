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
  
  DictO obj2;
  // set on all fields passed in except these
  auto fields = vector<string>{"type", "objtype", "me", "id", "name"};
  for (auto i: j.dict()) {
    auto key = get<0>(i);
    if (find(fields.begin(), fields.end(), key) == fields.end()) {
      obj2[key] = get<1>(i);
    }
  }

  L_TRACE("setting name " << (name ? name.value() : "(not)"));
  L_TRACE("setting obj " << Dict::toString(obj2));
  
  Handler::update(server, objtype.value(), id.value(), 
    j.getString("me", true), name, obj2);

}

};