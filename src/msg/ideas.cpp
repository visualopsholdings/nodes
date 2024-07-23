/*
  ideas.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

void Server::ideasMsg(json &j) {

  auto streamid = Json::getString(j, "stream");
  if (!streamid) {
    sendErr("no stream");
    return;
  }

  auto docs = Idea().find(json{ { "stream", streamid.value() } }).values();
  if (!docs) {
    boost::json::array empty;
    send({
      { "type", "ideas" },
      { "stream", streamid.value() },
      { "ideas", empty }
    });
    return;
  }

  boost::json::array s;
  for (auto i: docs.value()) {
    s.push_back(i.j());
  }

  send({
    { "type", "ideas" },
    { "stream", streamid.value() },
    { "ideas", s }
  });

}
