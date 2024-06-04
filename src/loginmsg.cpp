/*
  loginmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"

#include <boost/log/trivial.hpp>

void Server::loginMsg(json &j, shared_ptr<Storage> storage) {

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
  auto result = storage->coll("users").find({ { "name", password } }).value();
  if (!result) {
    sendErr("DB Error");
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << *result;
  string id;
  if (!getId(result, &id)) {
    sendErr("User not found");
    return;
  }

  string name;
  if (!getString(result, "name", &name)) {
    sendErr("No name in user");
    return;
  }
  string fullname;
  if (!getString(result, "fullname", &fullname)) {
    sendErr("No fullname in user");
    return;
  }
  
  send({
    { "type", "user" },
    { "session", session },
    { "id", id },
    { "name", name },
    { "fullname", fullname }
  });

}
