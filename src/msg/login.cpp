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

#include <boost/log/trivial.hpp>

void Server::loginMsg(json &j) {

  string session;
  if (!getString(j, "session", &session)) {
    sendErr("no session");
    return;
  }
  string password;
  if (!getString(j, "password", &password)) {
    sendErr("no password");
    return;
  }
  
  optional<UserRow> user;
  if (_test) {
    user = User().find(json{ { "name", password } }, {"name", "fullname"}).value();
    if (!user) {
      sendErr("DB Error");
      return;
    }
  }
  else {
    // use password.
    VID vid(password);
    user = User().findById(vid.uuid(), {"name", "fullname", "salt", "hash"}).value();
    if (!user) {
      sendErr("DB Error");
      return;
    }
    if (!Security::instance()->valid(vid, user->salt(), user->hash())) {
      sendErr("Incorrect password");
      return;
    }
  }
//  BOOST_LOG_TRIVIAL(trace) << user.value().j();
  
  send({
    { "type", "user" },
    { "session", session },
    { "id", user->id() },
    { "name", user->name() },
    { "fullname", user->fullname() }
  });

}
