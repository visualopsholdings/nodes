/*
  setgroupolicy.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 11-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void setGroupPolicyMsg(Server *server, const IncomingMsg &in) {

  if (!in.id) {
    server->sendErr("no id in group");
    return;
  }  
  
  Group groups;
  if (!Security::instance()->canEdit(groups, in.me, in.id.value())) {
    server->sendErr("no edit for groups " + in.id.value());
    return;
  }

  auto orig = groups.findById(in.id.value(), {}).one();
  if (!orig) {
    server->sendErr("group not found");
    return;
  }

  // build our add and remove vectors.
  vector<addTupleType> add;
  auto arr = Dict::getVector(in.extra_fields.get("add"));
  if (arr) {
    transform(arr->begin(), arr->end(), back_inserter(add), [](auto e) -> addTupleType {
      auto type = Dict::getStringG(e, "type");
      auto context = Dict::getStringG(e, "context");
      auto _id = Dict::getStringG(e, "_id");
      if (!(type && context && _id)) {
        L_ERROR("line missing type, context or _id");
        return addTupleType {};
      }
      return addTupleType { *type, *context, *_id }; 
    });
  }
  
  vector<string> remove;
  arr = Dict::getVector(in.extra_fields.get("remove"));
  if (arr) {
    transform(arr->begin(), arr->end(), back_inserter(remove), [](auto e) {
      auto s = Dict::getString(e);
      return s ? *s : "???";
    });
  }
  
  auto policy = Security::instance()->modifyPolicy(orig.value().policy(), add, remove);
  if (!policy) {
    server->sendErr("could not modify policy");
  }
  if (policy.value() == orig.value().policy()) {
    L_INFO("policy didn't change for " << in.id.value());
    server->sendAck();
  }
  
  auto obj = dictO({
    { "modifyDate", Storage::instance()->getNow() },
    { "policy", policy.value() }
  });

  // send to other nodes.
  auto obj2 = obj;
  if (orig.value().upstream()) {
    obj2["upstream"] = true;
  }
  server->sendUpd("group", in.id.value(), obj2);
    
  L_TRACE("updating " << Dict::toString(obj));
  auto result = groups.updateById(in.id.value(), obj);
  if (!result) {
    server->sendErr("could not update group");
    return;
  }
  
  L_TRACE("updated " << Dict::toString(result.value()));
  server->sendAck(result.value());

}

};