/*
  messagemsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"

#include <boost/log/trivial.hpp>

void Server::messageMsg(json &j, shared_ptr<Storage> storage) {

  string username;
  if (!getString(j, "user", &username)) {
    sendErr("no user");
    return;
  }

  auto user = User(*storage).find({ { "name", username } }).value();
  if (!user) {
    sendErr("DB Error");
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << user.value();
  string userid;
  if (!getId(user, &userid)) {
    sendErr("User not found");
    return;
  }

  string stream;
  if (!getString(j, "stream", &stream)) {
    sendErr("no stream");
    return;
  }
  if (stream != "s1") {
    sendErr("not correct stream");
    return;
  }
  string policy;
  if (!getString(j, "policy", &policy)) {
    sendErr("no policy");
    return;
  }
  if (policy != "p1") {
    sendErr("not correct policy");
    return;
  }
  string text;
  if (!getString(j, "text", &text)) {
    sendErr("no text");
    return;
  }
  if (text == "hello") {
    send({
      { "type", "message" },
      { "text", "world" },
      { "stream", "s1" },
      { "policy", "p1" },
      { "user", userid }
    });
    return;
  }
  BOOST_LOG_TRIVIAL(info) << "got " << text << " from " << userid;
  sendAck();

}
