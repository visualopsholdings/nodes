/*
  loginmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "vid.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void loginMsg(Server *server, Data &j) {

  auto session = j.getString("session");
  if (!session) {
    server->sendErr("no session");
    return;
  }
  auto password = j.getString("password");
  if (!password) {
    server->sendErr("no password");
    return;
  }
  
  optional<UserRow> user;
  // even if the server is in test mode, if the password seems like
  // a VID then try that out.
  if (server->_test && password.value().rfind("Vk9", 0) == string::npos) {
    Data q = { { "$or", { 
      { { "name", password.value() } },
      { { "fullname", password.value() } }
    } } };
    user = User().find(q, {"name", "fullname", "admin", "modifyDate"}).value();
    if (!user) {
      server->sendErr("Username/Password incorrect");
      return;
    }
  }
  else {
    // use password.
    VID vid(password.value());
    if (!vid.valid()) {
      server->sendErr("Invalid VID");
      return;
    }
    user = User().findById(vid.uuid(), {"name", "fullname", "salt", "hash", "admin", "active", "modifyDate"}).value();
    if (!user) {
      L_TRACE("couldn't find user");
      server->sendErr("Username/Password incorrect");
      return;
    }
    if (!user->active()) {
      L_TRACE("user not acitve");
      server->sendErr("Username/Password incorrect");
      return;
    }
    if (!Security::instance()->valid(vid, user->salt(), user->hash())) {
      L_TRACE("password incorrect salt(" << user->salt()  << ") hash(" << user->hash() << ")");
      server->sendErr("Username/Password incorrect");
      return;
    }
  }
//  L_TRACE(user.value().j();)
  
  server->send({
    { "type", "user" },
    { "session", session.value() },
    { "id", user->id() },
    { "name", user->name() },
    { "fullname", user->fullname() },
    { "admin", user->admin() },
    { "modifyDate", user->modifyDate() }
  });

}

};