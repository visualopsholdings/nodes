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
#include "handler.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void addObjectMsg(Server *server, Data &j) {

  auto objtype = j.getString("objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }

  auto upstream = j.getBool("upstream", true);
  if (upstream && upstream.value()) {

    auto id = j.getString("id");
    if (!id) {
      server->sendErr("no id");
      return;
    }
  
    Handler::upstream(server, objtype.value(), id.value(), "name");
    return;
  }

  auto me = j.getString("me");
  if (!me) {
    server->sendErr("no me");
    return;
  }
  
  Data obj = {{}};

  // if the object has a parent, then we use the security of the parent.
  string parentfield;
  string parenttype;
  string namefield;
  if (Storage::instance()->parentInfo(objtype.value(), &parentfield, &parenttype, &namefield)) {
    auto name = j.getString(namefield);
    if (!name) {
      server->sendErr("no " + namefield);
      return;
    }
    auto parentid = j.getString(parentfield);
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
    if (j.has("policy")) {
      policyid = j.getString("policy").value();
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
      auto policy = doc.value().getString("policy");
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
    auto name = j.getString("name");
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
  
  Handler::add(server, objtype.value(), obj, j.getString("corr", true));

}

};