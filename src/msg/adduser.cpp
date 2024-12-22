/*
  adduser.cpp
  
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
#include "date.hpp"
#include "handler.hpp"

#include "log.hpp"

namespace nodes {

void discoverLocalResultMsg(Server *server, json &);

void addUserMsg(Server *server, json &j) {
  
  auto upstream = Json::getBool(j, "upstream", true);
  if (upstream && upstream.value()) {
  
    auto id = Json::getString(j, "id");
    if (!id) {
      server->sendErr("no user id");
      return;
    }
  
    Handler::upstream(server, "user", id.value(), "fullname");
    return;
  }
  
  auto coll = Json::getString(j, "collection");
  if (!coll) {
    server->sendErr("no collection");
    return;
  }

  auto vopsidtoken = Json::getString(j, "vopsidtoken");
  if (!vopsidtoken) {
    server->sendErr("only upstream or having a token.");
    return;
  }
  auto fullname = Json::getString(j, "fullname");

  auto token = Security::instance()->expandShareToken(vopsidtoken.value());
  if (!token) {
    server->sendErr("Could not expand token");
    return;
  }
  
  L_TRACE("token " << token.value());

  auto expires = Json::getString(token.value(), "expires");
  if (!expires) {
    L_ERROR("Expires must be specified in token");
    server->sendWarning("Invalid token.");
    return;
  }
  
  auto tnow = Date::now();
  auto texpires = Date::fromISODate(expires.value());
  if (texpires < tnow) {
    L_ERROR("Token expired");
    L_TRACE("token expires " << expires.value() << ": " << texpires);
    L_TRACE("now " << Date::toISODate(tnow) << ": " << tnow);
    server->sendWarning("Invalid token.");
    return;
  }
  
  auto user = Json::getString(token.value(), "user");
  if (!user) {
    L_ERROR("User must be specified in token");
    server->sendWarning("Invalid token.");
    return;
  }

  auto id = Json::getString(token.value(), "id");
  if (!id) {
    L_ERROR("Id must be specified in token");
    server->sendWarning("Invalid token.");
    return;
  }

  auto result = SchemaImpl::findByIdGeneral(coll.value(), id.value(), {});
  if (!result) {
    server->sendErr("Can't find in " + coll.value());
    return;
  }
  if (!result->value()) {
    server->sendErr("collection in token doesn't exist");
    return;
  }

  boost::json::object obj = {
    { "invitedBy", user.value() },
    { "active", true },
    { "modifyDate", Storage::instance()->getNow() }
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
    L_ERROR("Security settings not supported");
    server->sendWarning("Invalid token.");
    return;
  }
  
  string salt = Security::instance()->newSalt();
  string password = Security::instance()->newPassword();
  string hash = Security::instance()->newHash(password, salt);

  obj["salt"] = salt;
  obj["hash"] = hash;

  auto userid = User().insert(obj);
  if (!userid) {
    server->sendErr("could not insert user");
    return;
  }

  // send to other nodes.
  obj["id"] = userid.value();
  server->sendAdd("user", obj);
    
  VID vid;
  vid.reset(userid.value(), password);
  
  if (!Group().addMember(team.value(), userid.value())) {
     server->sendErr("could not add user to team");
     return;
  }
  Security::instance()->regenerateGroups();

  server->send({
    { "type", "adduser" },
    { "vopsid", vid.value() },
    { "id", userid.value() },
    { "fullname", fullname.value() }
  });

}

};