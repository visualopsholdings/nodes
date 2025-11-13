/*
  moveobject.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 29-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "handler.hpp"
#include "log.hpp"

namespace nodes {

void moveObjectMsg(Server *server, const IncomingMsg &in) {

  if (!in.objtype) {
    server->sendErr("no object type");
    return;
  }

  string parentfield;
  string parenttype;
  if (!Storage::instance()->parentInfo(in.objtype.value(), &parentfield, &parenttype)) {
    server->sendErr("object has no parent");
    return;
  }

  // get the parent collection name.
  string pcoll;
  if (!Storage::instance()->collName(parenttype, &pcoll)) {
    server->sendErr("Could not get collection name for move object");
    return;
  }
  
  auto pcollid = Dict::getString(in.extra_fields.get("coll"));
  if (!pcollid) {
    server->sendErr("no coll");
    return;
  }

  if (!SchemaImpl::existsGeneral(pcoll, pcollid.value())) {
    server->sendErr("collection does not exist to move to " + pcollid.value());
    return;
  }
  
  if (!in.id) {
    server->sendErr("no id");
    return;
  }

  if (!in.me) {
    server->sendErr("no me");
    return;
  }
  
  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(in.objtype.value(), &coll, false)) {
    server->sendErr("Could not get collection name for moveObjectMsg");
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
  
  L_TRACE(in.objtype.value() << " old value " << Dict::toString(orig.value()));
  
  auto origparent = Dict::getString(orig.value(), parentfield);
  if (!origparent) {
    server->sendErr("parent field not found");
    return;
  }

  if (!Security::instance()->canEdit(pcoll, in.me, origparent.value())) {
    L_ERROR("no edit for " << in.objtype.value() << " " << in.id.value());
    server->sendSecurity();
    return;
  }
  
  // Set the parent field of the object.
  auto obj = dictO({
    { parentfield, pcollid.value() }
  });
  obj["modifyDate"] = Storage::instance()->getNow();
  
  // send to other nodes.
  auto obj2 = obj;
  auto upstream = Dict::getBool(orig.value(), "upstream");
  if (upstream) {
    obj2["upstream"] = *upstream;
  }
  server->sendMov(in.objtype.value(), in.id.value(), obj2, parenttype, origparent.value());
    
  // update locally
  L_TRACE("updating " << Dict::toString(obj));
  auto r = SchemaImpl::updateGeneralById(coll, in.id.value(), dictO({
    { "$set", obj }
  }));
  if (!r) {
    server->sendErr("could not update " + in.objtype.value());
    return;
  }
  
  // and reply back
  L_TRACE("updated " << r.value());
  server->sendAck();

}

};