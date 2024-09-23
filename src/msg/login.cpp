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
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void loginMsg(Server *server, json &j) {

  auto session = Json::getString(j, "session");
  if (!session) {
    server->sendErr("no session");
    return;
  }
  auto password = Json::getString(j, "password");
  if (!password) {
    server->sendErr("no password");
    return;
  }
  
  optional<UserRow> user;
  // even if the server is in test mode, if the password seems like
  // a VID then try that out.
  if (server->_test && password.value().rfind("Vk9", 0) == string::npos) {
    json q = { { "$or", { 
      { { "name", password.value() } },
      { { "fullname", password.value() } }
    } } };
    user = User().find(q, {"name", "fullname", "admin"}).value();
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
    user = User().findById(vid.uuid(), {"name", "fullname", "salt", "hash", "admin", "active"}).value();
    if (!user) {
      BOOST_LOG_TRIVIAL(trace) << "couldn't find user";
      server->sendErr("Username/Password incorrect");
      return;
    }
    if (!user->active()) {
      BOOST_LOG_TRIVIAL(trace) << "user not acitve";
      server->sendErr("Username/Password incorrect");
      return;
    }
    if (!Security::instance()->valid(vid, user->salt(), user->hash())) {
      BOOST_LOG_TRIVIAL(trace) << "password incorrect salt(" << user->salt()  << ") hash(" << user->hash() << ")";
      server->sendErr("Username/Password incorrect");
      return;
    }
  }
//  BOOST_LOG_TRIVIAL(trace) << user.value().j();
  
  server->send({
    { "type", "user" },
    { "session", session.value() },
    { "id", user->id() },
    { "name", user->name() },
    { "fullname", user->fullname() },
    { "admin", user->admin() }
  });

}

};