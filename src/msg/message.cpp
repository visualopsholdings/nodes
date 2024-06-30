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

void Server::messageMsg(json &j) {

  string userid;
  if (!getString(j, "user", &userid)) {
    sendErr("no user");
    return;
  }

  if (!User().findById(userid, { "id" }).value()) {
    sendErr("User not found");
    return;
  }
  
  string streamid;
  if (!getString(j, "stream", &streamid)) {
    sendErr("no stream");
    return;
  }
  
//   if (!Stream().findById(streamid, { "id" }).value()) {
//     sendErr("Stream not found");
//     return;
//   }

  string policyid;
  if (!getString(j, "policy", &policyid)) {
    sendErr("no policy");
    return;
  }

//   if (!Policy().findById(policyid, { "id" }).value()) {
//     sendErr("Policy not found");
//     return;
//   }

  string text;
  if (!getString(j, "text", &text)) {
    sendErr("no text");
    return;
  }
  
  json idea = {
    { "user", userid },
    { "stream", streamid },
    { "policy", policyid },
    { "text", text}
  };
  auto result = Idea().insert(idea);
  if (!result) {
    sendErr("DB Error");
    return;
  }
  BOOST_LOG_TRIVIAL(trace) << "inserted " << *result;
    
  publish(idea);
  sendAck();

}
