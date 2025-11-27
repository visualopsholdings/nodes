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

namespace nodes {

void setObjectMsg(Server *server, const IncomingMsg &in) {

  if (!in.objtype) {
    server->sendErr("no object type");
    return;
  }
  if (!in.id) {
    server->sendErr("no id in set object");
    return;
  }  
  
//  L_TRACE(j);
  
  auto name = Dict::getString(in.extra_fields.get("name"));
  
  DictO obj2;
  
  // set on all fields passed in except these
  auto fields = vector<string>{"type", "objtype", "me", "id", "name"};
  auto obj = server->toObject(in);
  if (!obj) {
    server->sendErr("can't convert to object!");
    return;
  }
  for (auto i: *obj) {
    auto key = get<0>(i);
    if (find(fields.begin(), fields.end(), key) == fields.end()) {
      obj2[key] = get<1>(i);
    }
  }

  L_TRACE("setting name " << (name ? name.value() : "(not)"));
  L_TRACE("setting obj " << Dict::toString(obj2));
  
  Handler::update(server, in.objtype.value(), in.id.value(), 
    in.me, name, obj2);

}

};