/*
  messagemsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

void Server::messageMsg(json &j) {

  auto userid = Json::getString(j, "user");
  if (!userid) {
    sendErr("no user");
    return;
  }

  if (!User().findById(userid.value(), { "id" }).value()) {
    sendErr("User not found");
    return;
  }
  
  auto streamid = Json::getString(j, "stream");
  if (!streamid) {
    sendErr("no stream");
    return;
  }
  
//   if (!Stream().findById(streamid.value(), { "id" }).value()) {
//     sendErr("Stream not found");
//     return;
//   }

  auto policyid = Json::getString(j, "policy");
  if (!policyid) {
    sendErr("no policy");
    return;
  }

//   if (!Policy().findById(policyid.value(), { "id" }).value()) {
//     sendErr("Policy not found");
//     return;
//   }

  auto text = Json::getString(j, "text");
  if (!text) {
    sendErr("no text");
    return;
  }
  
  json idea = {
    { "user", userid.value() },
    { "stream", streamid.value() },
    { "policy", policyid.value() },
    { "text", text.value() }
  };
  auto result = Idea().insert(idea);
  if (!result) {
    sendErr("DB Error");
    return;
  }
  BOOST_LOG_TRIVIAL(trace) << "inserted " << result.value();
    
  publish(idea);
  sendAck();

}
