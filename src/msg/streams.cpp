/*
  streamsmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"

#include <boost/log/trivial.hpp>

void Server::streamsMsg(json &j, shared_ptr<Storage> storage) {

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

  send({
    { "type", "streams" },
    { "user", userid },
    { "streams", {
      { { "name", "My Conversation 1" }, { "id", "s1" }, { "policy", "p1" } },
      { { "name", "My Conversation 2" }, { "id", "s2" }, { "policy", "p2" } }
      } 
    }
  });

}
