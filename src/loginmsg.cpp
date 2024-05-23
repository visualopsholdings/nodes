/*
  loginmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include <boost/log/trivial.hpp>

void Server::loginMsg(json *j) {

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
  if (password == "tracy") {
    send({
      { "type", "user" },
      { "session", session },
      { "id", "u1" },
      { "name", "tracy" },
      { "fullname", "Tracy" }
    });
    return;
  }
  if (password == "leanne") {
    send({
      { "type", "user" },
      { "session", session },
      { "id", "u2" },
      { "name", "leanne" },
      { "fullname", "Leanne" }
    });
    return;
  }
  send({ { "type", "err" }, { "msg", "User not found" } });

}
