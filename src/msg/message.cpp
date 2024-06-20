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

  string userid;
  if (!getString(j, "user", &userid)) {
    sendErr("no user");
    return;
  }

  auto user = User(*storage).findById(userid, { "id" }).value();
  if (!user) {
    sendErr("DB Error");
    return;
  }
  
//  BOOST_LOG_TRIVIAL(trace) << user.value();
  
  string streamid;
  if (!getString(j, "stream", &streamid)) {
    sendErr("no stream");
    return;
  }
  
//   auto stream = Stream(*storage).findById(streamid, { "id" }).value();
//   if (!stream) {
//     sendErr("DB Error");
//     return;
//   }

  string policyid;
  if (!getString(j, "policy", &policyid)) {
    sendErr("no policy");
    return;
  }

//   auto policy = Policy(*storage).findById(policyid, { "id" }).value();
//   if (!stream) {
//     sendErr("DB Error");
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
  auto result = Idea(*storage).insert(idea);
  if (!result) {
    sendErr("DB Error");
    return;
  }
  BOOST_LOG_TRIVIAL(trace) << "inserted " << *result;
    
  publish(idea);
  sendAck();

}
