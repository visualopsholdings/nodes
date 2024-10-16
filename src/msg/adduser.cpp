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

#include <boost/log/trivial.hpp>

namespace nodes {

void discoverLocalResultMsg(Server *server, json &);

void addUserMsg(Server *server, json &j) {

  auto upstream = Json::getBool(j, "upstream", true);
  if (upstream && upstream.value()) {

    auto userid = Json::getString(j, "id");
    if (!userid) {
      server->sendErr("no userid");
      return;
    }
  
    auto doc = User().findById(userid.value()).value();
    if (doc) {
      // set the upstream on doc.
      auto result = User().updateById(userid.value(), { 
        { "modifyDate", Storage::instance()->getNow() },
        { "upstream", true } 
      });
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
    server->sendUpDiscover();
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
  
  BOOST_LOG_TRIVIAL(trace) << "token " << token.value();

  auto expires = Json::getString(token.value(), "expires");
  if (!expires) {
    BOOST_LOG_TRIVIAL(error) << "Expires must be specified in token";
    server->sendWarning("Invalid token.");
    return;
  }
  
  auto tnow = Date::now();
  auto texpires = Date::fromISODate(expires.value());
  if (texpires < tnow) {
    BOOST_LOG_TRIVIAL(error) << "Token expired";
    BOOST_LOG_TRIVIAL(trace) << "stream token expires " << expires.value() << ": " << texpires;
    BOOST_LOG_TRIVIAL(trace) << "now " << Date::toISODate(tnow) << ": " << tnow;
    server->sendWarning("Invalid token.");
    return;
  }
  
  auto user = Json::getString(token.value(), "user");
  if (!user) {
    BOOST_LOG_TRIVIAL(error) << "User must be specified in token";
    server->sendWarning("Invalid token.");
    return;
  }

  auto streamid = Json::getString(token.value(), "id");
  if (!streamid) {
    BOOST_LOG_TRIVIAL(error) << "Stream must be specified in token";
    server->sendWarning("Invalid token.");
    return;
  }

  auto stream = Stream().findById(streamid.value()).value();
  if (!stream) {
    server->sendErr("Stream in token doesn't exist");
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
    BOOST_LOG_TRIVIAL(error) << "Security settings not supported";
    server->sendWarning("Invalid token.");
    return;
  }
  
  string salt = Security::instance()->newSalt();
  string password = Security::instance()->newPassword();
  string hash = Security::instance()->newHash(password, salt);

  obj["salt"] = salt;
  obj["hash"] = hash;

  auto id = User().insert(obj);
  if (!id) {
    server->sendErr("could not insert user");
    return;
  }

  // send to other nodes.
  obj["id"] = id.value();
  server->sendAdd("user", obj);
    
  VID vid;
  vid.reset(id.value(), password);
  
//   vector<tuple<string, string, string > > add;
//   add.push_back({ "view", "user", id.value() });
//   add.push_back({ "edit", "user", id.value() });
//   add.push_back({ "exec", "user", id.value() });
//   vector<string> remove;
//   auto newpolicy = Security::instance()->modifyPolicy(stream.value().policy(), add, remove);
//   if (newpolicy.value() != stream.value().policy()) {
//     BOOST_LOG_TRIVIAL(trace) << "new policy needed for stream" << newpolicy.value();
//     Stream().updateById(stream.value().id(), { { "policy", newpolicy.value() }});
//   }
  
  if (!Group().addMember(team.value(), id.value())) {
     server->sendErr("could not add user to team");
     return;
  }
  Security::instance()->regenerateGroups();

  server->send({
    { "type", "adduser" },
    { "vopsid", vid.value() },
    { "id", id.value() },
    { "fullname", fullname.value() }
  });

}

};