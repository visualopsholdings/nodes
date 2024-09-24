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
#include "security.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void messageMsg(Server *server, json &j) {

  auto me = Json::getString(j, "me");
  if (!me) {
    server->sendErr("no user");
    return;
  }

  auto streamid = Json::getString(j, "stream");
  if (!streamid) {
    server->sendErr("no stream");
    return;
  }
  
  Stream streams;
  if (!Security::instance()->canEdit(streams, me, streamid.value())) {
    server->sendErr("can't edit stream " + streamid.value());
    return;
  }

//   if (!Stream().findById(streamid.value(), { "id" }).value()) {
//     server->sendErr("Stream not found");
//     return;
//   }

  string policyid;
  if (Json::has(j, "policy")) {
    policyid = Json::getString(j, "policy").value();
  }
  else {
    auto doc = streams.findById(streamid.value(), {"policy"}).value();
    if (!doc) {
      server->sendErr("invalid stream");
      return;
    }
    policyid = doc->policy();
  }

//   if (!Policy().findById(policyid.value(), { "id" }).value()) {
//     server->sendErr("Policy not found");
//     return;
//   }

  auto text = Json::getString(j, "text");
  if (!text) {
    server->sendErr("no text");
    return;
  }
  
  json idea = {
    { "user", me.value() },
    { "stream", streamid.value() },
    { "policy", policyid },
    { "text", text.value() },
    { "modifyDate", Storage::instance()->getNow() }
  };
  auto result = Idea().insert(idea);
  if (!result) {
    server->sendErr("DB Error (no idea)");
    return;
  }
  BOOST_LOG_TRIVIAL(trace) << "inserted " << result.value();
    
  idea.as_object()["id"] = result.value();
  idea.as_object()["type"] = "idea";
  
  if (!Json::has(j, "corr")) {
    BOOST_LOG_TRIVIAL(warning) << "message missing correlation id";
  }
  
  server->publish(Json::getString(j, "corr", true), idea);

  server->sendAck();

}

};