/*
  setobjectpolicy.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void setObjectPolicyMsg(Server *server, const IncomingMsg &in) {

  if (!in.objtype) {
    server->sendErr("no object type");
    return;
  }
  if (!in.id) {
    server->sendErr("no id in " + in.objtype.value());
    return;
  }  
  
  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(in.objtype.value(), &coll)) {
    server->sendErr("Could not get collection name for setobjectpolicy");
    return;
  }
  
  if (!Security::instance()->canEdit(coll, in.me, in.id.value())) {
    server->sendErr("no edit for " + in.objtype.value() + " " + in.id.value());
    return;
  }

  auto result = SchemaImpl::findByIdGeneral(coll, in.id.value(), {});
  if (!result) {
    server->sendErr(in.objtype.value() + " can't find");
    return;
  }
  auto orig = result->value();
  if (!orig) {
    server->sendErr(in.objtype.value() + " not found");
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
  
  auto policyid = Dict::getString(orig.value(), "policy");
  if (!policyid) {
    server->sendErr(in.objtype.value() + " no policy id");
    return;
  }
  
  auto policy = Security::instance()->modifyPolicy(policyid.value(), add, remove);
  if (!policy) {
    server->sendErr("could not modify policy");
  }
  if (policy.value() == policyid.value()) {
    L_INFO("policy didn't change for " << in.id.value());
    server->sendAck();
  }
  
  auto obj = dictO({
    { "modifyDate", Storage::instance()->getNow() },
    { "policy", policy.value() }
  });

  // send to other nodes.
  auto obj2 = obj;
  auto upstream = Dict::getBool(orig.value(), "upstream");
  if (upstream) {
    obj2["upstream"] = *upstream;
  }
  server->sendUpd(in.objtype.value(), in.id.value(), obj2);
    
  L_TRACE("updating " << Dict::toString(obj));
  auto r = SchemaImpl::updateGeneralById(coll, in.id.value(), dictO({
    { "$set", obj }
  }));
  
  L_TRACE("updated " << Dict::toString(r.value()));
  server->sendAck(r.value());

}

};