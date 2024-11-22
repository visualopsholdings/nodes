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
  
  if (!Security::instance()->canEdit("streams", me, streamid.value())) {
    server->sendErr("no edit for stream " + streamid.value());
    return;
  }

  string policyid;
  if (Json::has(j, "policy")) {
    policyid = Json::getString(j, "policy").value();
  }
  else {
    auto result = SchemaImpl::findByIdGeneral("streams", streamid.value(), {"policy"});
    if (!result) {
      server->sendErr("Can't find stream.");
      return;
    }
    auto doc = result->value();
    if (!doc) {
      server->sendErr("invalid stream");
      return;
    }
    BOOST_LOG_TRIVIAL(trace) << doc.value();
    auto policy = Json::getString(doc.value(), "policy");
    if (!policy) {
      server->sendErr("stream missing policy");
      return;
    }
    policyid = policy.value();
  }

  auto text = Json::getString(j, "text");
  if (!text) {
    server->sendErr("no text");
    return;
  }
  
  boost::json::object obj = {
    { "user", me.value() },
    { "stream", streamid.value() },
    { "policy", policyid },
    { "text", text.value() },
    { "modifyDate", Storage::instance()->getNow() }
  };
  auto id = SchemaImpl::insertGeneral("ideas", obj);
  if (!id) {
    server->sendErr("DB Error (no idea)");
    return;
  }
  BOOST_LOG_TRIVIAL(trace) << "inserted " << id.value();
    
  obj["id"] = id.value();
  obj["type"] = "idea";
  
  // send to other nodes.
  server->sendAdd("idea", obj);

  // publish as well
  if (!Json::has(j, "corr")) {
    BOOST_LOG_TRIVIAL(warning) << "message missing correlation id";
  }
  server->publish(Json::getString(j, "corr", true), obj);
  
  server->sendAck(id.value());

}

};