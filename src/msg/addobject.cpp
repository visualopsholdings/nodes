/*
  addobject.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"
#include "handler.hpp"

#include "log.hpp"

namespace nodes {

void addObjectMsg(Server *server, json &j) {

  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }

  auto upstream = Json::getBool(j, "upstream", true);
  if (upstream && upstream.value()) {

    auto id = Json::getString(j, "id");
    if (!id) {
      server->sendErr("no id");
      return;
    }
  
    Handler::upstream(server, objtype.value(), id.value(), "name");
    return;
  }

  auto me = Json::getString(j, "me");
  if (!me) {
    server->sendErr("no me");
    return;
  }
  
  json obj;

  // if the object has a parent, then we use the security of the parent.
  string parentfield;
  string parenttype;
  string namefield;
  if (Storage::instance()->parentInfo(objtype.value(), &parentfield, &parenttype, &namefield)) {
    auto name = Json::getString(j, namefield);
    if (!name) {
      server->sendErr("no " + namefield);
      return;
    }
    auto parentid = Json::getString(j, parentfield);
    if (!parentid) {
      server->sendErr("no " + parentfield);
      return;
    }
    string pcoll;
    if (!Storage::instance()->collName(parenttype, &pcoll, false)) {
      server->sendErr("couldn't get collection name for  " + parenttype);
      return;
    }
    if (!Security::instance()->canEdit(pcoll, me, parentid.value())) {
      server->sendErr("no edit for " + parenttype + " " + parentid.value());
      return;
    }
    string policyid;
    if (Json::has(j, "policy")) {
      policyid = Json::getString(j, "policy").value();
    }
    else {
      auto result = SchemaImpl::findByIdGeneral(pcoll, parentid.value(), {"policy"});
      if (!result) {
        server->sendErr("Can't find " + parenttype);
        return;
      }
      auto doc = result->value();
      if (!doc) {
        server->sendErr("invalid " + parenttype);
        return;
      }
      L_TRACE(doc.value());
      auto policy = Json::getString(doc.value(), "policy");
      if (!policy) {
        server->sendErr(parenttype + " missing policy");
        return;
      }
      policyid = policy.value();
    }

    // how do we know to add user and not active?
    obj = {
      { namefield, name.value() },
      { "policy", policyid },
      { "modifyDate", Storage::instance()->getNow() },
      { parentfield, parentid.value() },
      { "user", me.value() }
    };
  }
  else {
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
    
    obj = {
      { "name", name.value() },
      { "policy", policy.value() },
      { "modifyDate", Storage::instance()->getNow() },
      { "active", true }
    };
  }
  
  Handler::add(server, objtype.value(), obj, Json::getString(j, "corr", true));

}

};