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

  auto user = User(*storage).find(json{ { "name", username } }, { "_id" }).value();
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

  auto streams = Stream(*storage).find({{}}, { "_id", "name", "policy" }).values();
  if (!streams) {
    sendErr("DB Error");
    return;
  }
  BOOST_LOG_TRIVIAL(trace) << streams.value();
  
  send({
    { "type", "streams" },
    { "user", userid },
    { "streams", streams.value() }
  });

}
