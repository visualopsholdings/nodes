/*
  newuser.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 20-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "security.hpp"
#include "vid.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void discoverLocalResultMsg(Server *server, json &);

void newUserMsg(Server *server, json &j) {

  auto upstream = Json::getBool(j, "upstream");
  if (upstream && upstream.value()) {

    auto userid = Json::getString(j, "id");
    if (!userid) {
      server->sendErr("no userid");
      return;
    }
  
    auto doc = User().find(json{ { "_id", { { "$oid", userid.value() } } } }, {}).value();
    if (doc) {
      // set the upstream on doc.
      auto result = User().updateById(userid.value(), {{ "upstream", true }});
      if (!result) {
        server->sendErr("could not update user");
        return;
      }
      server->sendAck();
      return;
    }
    
    // insert a new user
    auto result = User().insert({
      { "_id", { { "$oid", userid.value() } } },
      { "fullname", "Waiting discovery" },
      { "upstream", true },
      { "modifyDate", { { "$date", 0 } } }
    });
    if (!result) {
      server->sendErr("could not insert user");
      return;
    }
    
    server->sendAck();
    
    // run discovery.  
    server->discover();
    return;
  }
  
  auto vopsidtoken = Json::getString(j, "vopsidtoken");
  if (!vopsidtoken) {
    server->sendErr("only upstream or having a token.");
    return;
  }
  auto fullname = Json::getString(j, "fullname");

  auto token = Security::instance()->expandStreamShareToken(vopsidtoken.value());
  if (!token) {
    server->sendErr("Could not expand token");
    return;
  }
  
  auto expires = Json::getString(token.value(), "expires");
  if (!expires) {
    BOOST_LOG_TRIVIAL(error) << "Expires must be specified in token";
    server->sendWarning("Invalid token.");
    return;
  }
  
  auto user = Json::getString(token.value(), "user");
  if (!user) {
    BOOST_LOG_TRIVIAL(error) << "User must be specified in token";
    server->sendWarning("Invalid token.");
    return;
  }
  // TBD: Test for expiry...
  
  boost::json::object obj = {
    { "invitedBy", user.value() },
  };

  auto options = Json::getString(token.value(), "options");
  if (options && options.value() == "mustName") {
    if (!fullname || fullname.value().size() == 0) {
      server->sendWarning("Must specify full name." );
      return;
    }
    obj["fullname"] = fullname.value();
  }
  
  auto team = Json::getString(token.value(), "team");
  if (!team) {
    BOOST_LOG_TRIVIAL(error) << "Security settings not supported";
    server->sendWarning("Invalid token.");
    return;
  }
  
  auto salt = Security::instance()->newSalt();
  if (!salt) {
    server->sendErr("Cant create new Salt");
    return;
  }
  
  string password = Security::instance()->newPassword();
  
  auto hash = Security::instance()->newHash(password, salt.value());
  if (!hash) {
    server->sendErr("Cant create new hash for user");
    return;
  }

  obj["salt"] = salt.value();
  obj["hash"] = hash.value();

  auto result = User().insert(obj);
  if (!result) {
    server->sendErr("could not insert user");
    return;
  }

  VID vid;
  vid.reset(result.value(), password);
  
  // TBD: Add security.

  server->send({
    { "type", "newuser" },
    { "vopsid", vid.value() },
    { "fullname", fullname.value() }
  });

}

};