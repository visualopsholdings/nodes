/*
  loginmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"
#include "vid.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

void Server::loginMsg(json &j) {

  auto session = Json::getString(j, "session");
  if (!session) {
    sendErr("no session");
    return;
  }
  auto password = Json::getString(j, "password");
  if (!password) {
    sendErr("no password");
    return;
  }
  
  optional<UserRow> user;
  if (_test) {
    user = User().find(json{ { "name", password.value() } }, {"name", "fullname"}).value();
    if (!user) {
      sendErr("Username/Password incorrect");
      return;
    }
  }
  else {
    // use password.
    VID vid(password.value());
    if (!vid.valid()) {
      sendErr("Invalid VID");
      return;
    }
    user = User().findById(vid.uuid(), {"name", "fullname", "salt", "hash"}).value();
    if (!user) {
      sendErr("Username/Password incorrect");
      return;
    }
    if (!Security::instance()->valid(vid, user->salt(), user->hash())) {
      sendErr("Username/Password incorrect");
      return;
    }
  }
//  BOOST_LOG_TRIVIAL(trace) << user.value().j();
  
  send({
    { "type", "user" },
    { "session", session.value() },
    { "id", user->id() },
    { "name", user->name() },
    { "fullname", user->fullname() }
  });

}
