/*
  setuser.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void setuserMsg(Server *server, const IncomingMsg &in) {

  if (!in.id) {
    server->sendErr("no id in user");
    return;
  }  
  
  auto doc = User().findById(*in.id, {}).one();
  if (!doc) {
    server->sendErr("could not find user " + *in.id);
    return;
  }
  
  L_TRACE("user old value " << Dict::toString(doc->dict()));
  
  auto fullname = Dict::getString(in.extra_fields.get("fullname"));
  auto name = Dict::getString(in.extra_fields.get("name"));
  auto admin = Dict::getBool(in.extra_fields.get("admin"));
  auto active = Dict::getBool(in.extra_fields.get("active"));

  auto obj = dictO({
    { "modifyDate", Storage::instance()->getNow() },
  });
  if (fullname) {
    obj["fullname"] = *fullname;
  }    
  if (name) {
    obj["name"] = *name;
  }    
  if (active) {
    obj["active"] = *active;
  }    
  if (admin) {
    obj["admin"] = *admin;
  }
  
  // send to other nodes.
  auto obj2 = obj;
  if (doc->upstream()) {
    obj2["upstream"] = true;
  }
  server->sendUpd("user", *in.id, obj2);
  
  // update this node.
  L_TRACE("updating " << Dict::toString(obj));
  auto result = User().updateById(*in.id, obj);
  if (!result) {
    server->sendErr("could not update user");
    return;
  }
  L_TRACE("updated " << result.value());
  
  server->sendAck(result.value());
  
}

};