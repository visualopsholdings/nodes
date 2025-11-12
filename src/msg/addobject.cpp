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

void addObjectMsg(Server *server, const IncomingMsg &in) {

  if (!in.objtype) {
    server->sendErr("no object type");
    return;
  }

  auto upstream = Dict::getBool(in.extra_fields.get("upstream"));
  if (upstream && upstream.value()) {

    if (!in.id) {
      server->sendErr("no id");
      return;
    }
  
    Handler::upstream(server, in.objtype.value(), in.id.value(), "name");
    return;
  }

  if (!in.me) {
    server->sendErr("no me");
    return;
  }
  
  DictO obj;

  // if the object has a parent, then we use the security of the parent.
  string parentfield;
  string parenttype;
  string namefield;
  if (Storage::instance()->parentInfo(in.objtype.value(), &parentfield, &parenttype, &namefield)) {
    auto name = Dict::getString(in.extra_fields.get(namefield));
    if (!name) {
      server->sendErr("no " + namefield);
      return;
    }
    auto parentid = Dict::getString(in.extra_fields.get(parentfield));
    if (!parentid) {
      server->sendErr("no " + parentfield);
      return;
    }
    string pcoll;
    if (!Storage::instance()->collName(parenttype, &pcoll, false)) {
      server->sendErr("couldn't get collection name for  " + parenttype);
      return;
    }
    if (!Security::instance()->canEdit(pcoll, in.me, parentid.value())) {
      server->sendErr("no edit for " + parenttype + " " + parentid.value());
      return;
    }
    string policyid;
    auto policy = Dict::getString(in.extra_fields.get("policy"));
    if (policy) {
      policyid = *policy;
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
      L_TRACE(Dict::toString(doc.value()));
      auto policy = Dict::getString(doc.value(), "policy");
      if (!policy) {
        server->sendErr(parenttype + " missing policy");
        return;
      }
      policyid = policy.value();
    }

    // how do we know to add user and not active?
    obj = dictO({
      { namefield, name.value() },
      { "policy", policyid },
      { "modifyDate", Storage::instance()->getNowO() },
      { parentfield, parentid.value() },
      { "user", in.me.value() }
    });
  }
  else {
    auto name = Dict::getString(in.extra_fields.get("name"));
    if (!name) {
      server->sendErr("no name");
      return;
    }
    auto policy = Security::instance()->findPolicyForUser(in.me.value());
    if (!policy) {
      server->sendErr("could not get policy");
      return;
    }
    
    obj = dictO({
      { "name", name.value() },
      { "policy", policy.value() },
      { "modifyDate", Storage::instance()->getNowO() },
      { "active", true }
    });
  }
  
  Handler::add(server, in.objtype.value(), obj, in.corr);

}

};