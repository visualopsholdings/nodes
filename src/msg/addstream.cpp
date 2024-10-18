/*
  addstream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void addStreamMsg(Server *server, json &j) {

  auto upstream = Json::getBool(j, "upstream", true);
  if (upstream && upstream.value()) {

    auto streamid = Json::getString(j, "id");
    if (!streamid) {
      server->sendErr("no stream id");
      return;
    }
  
    auto doc = Stream().findById(streamid.value()).value();
    if (doc) {
      // set the upstream on doc.
      auto result = Stream().updateById(streamid.value(), { 
        { "modifyDate", Storage::instance()->getNow() },
        { "upstream", true } 
      });
      if (!result) {
        server->sendErr("could not update stream");
        return;
      }
      server->sendAck();
      return;
    }
    
    // insert a new stream
    auto result = Stream().insert({
      { "_id", { { "$oid", streamid.value() } } },
      { "name", "Waiting discovery" },
      { "upstream", true },
      { "modifyDate", { { "$date", 0 } } }
    });
    if (!result) {
      server->sendErr("could not insert stream");
      return;
    }
    
    server->sendAck();
    
    // run discovery.  
    server->sendUpDiscover();
    return;
  }

  auto me = Json::getString(j, "me");
  if (!me) {
    server->sendErr("no me");
    return;
  }
  auto name = Json::getString(j, "name");
  if (!name) {
    server->sendErr("no name");
    return;
  }
  
  auto policy = Security::instance()->findPolicyForUser(me.value());
  if (!policy) {
    server->sendErr("could not get policy");
    return;
  }
  
  boost::json::object obj = {
    { "name", name.value() },
    { "policy", policy.value() },
    { "modifyDate", Storage::instance()->getNow() },
    { "active", true }
  };
  
  // insert a new strean
  auto id = Stream().insert(obj);
  if (!id) {
    server->sendErr("could not insert stream");
    return;
  }
  
  // send to other nodes.
  obj["id"] = id.value();
  server->sendAdd("stream", obj, "");
    
  server->sendAck();

}

};