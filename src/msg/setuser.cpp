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
#include "data.hpp"

namespace nodes {

void setuserMsg(Server *server, Data &j) {

  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no id in user");
    return;
  }  
  
  auto doc = User().findById(id.value(), {}).value();
  if (!doc) {
    server->sendErr("could not find user " + id.value());
    return;
  }
  
  L_TRACE("user old value " << doc.value().d());
  
  auto fullname = j.getString("fullname", true);
  auto name = j.getString("name", true);
  auto admin = j.getBool("admin", true);
  auto active = j.getBool("active", true);

  Data obj = {
    { "modifyDate", Storage::instance()->getNow() },
  };
  if (fullname) {
    obj.setString("fullname", fullname.value());
  }    
  if (name) {
    obj.setString("name", name.value());
  }    
  if (active) {
    obj.setBool("active", active.value());
  }    
  if (admin) {
    obj.setBool("admin", admin.value());
  }
  
  // send to other nodes.
  Data obj2 = obj;
  if (doc.value().upstream()) {
    obj2.setBool("upstream", true);
  }
  server->sendUpd("user", id.value(), obj2);
  
  // update this node.
  L_TRACE("updating " << obj);
  auto result = User().updateById(id.value(), obj);
  if (!result) {
    server->sendErr("could not update user");
    return;
  }
  L_TRACE("updated " << result.value());
  
  server->sendAck(result.value());
  
}

};