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

void Server::loginMsg(json *j, shared_ptr<Storage> storage) {

  string session;
  if (!getString(j, "session", &session)) {
    BOOST_LOG_TRIVIAL(error) << "no session";
    return;
  }
  string password;
  if (!getString(j, "password", &password)) {
    BOOST_LOG_TRIVIAL(error) << "no password";
    return;
  }
  json result = storage->getUser(password);
  BOOST_LOG_TRIVIAL(trace) << result;
  string id;
  if (!getId(&result, &id)) {
    send({ { "type", "err" }, { "msg", "User not found" } });
    return;
  }

  string name;
  if (!getString(&result, "name", &name)) {
    send({ { "type", "err" }, { "msg", "No name in user" } });
    return;
  }
  string fullname;
  if (!getString(&result, "fullname", &fullname)) {
    send({ { "type", "err" }, { "msg", "No fullname in user" } });
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
