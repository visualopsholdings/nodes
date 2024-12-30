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

void moveObjectMsg(Server *server, Data &j) {

  auto objtype = j.getString("objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }

  string parentfield;
  string parenttype;
  if (!Storage::instance()->parentInfo(objtype.value(), &parentfield, &parenttype)) {
    server->sendErr("object has no parent");
    return;
  }

  // get the parent collection name.
  string pcoll;
  if (!Storage::instance()->collName(parenttype, &pcoll)) {
    server->sendErr("Could not get collection name for move object");
    return;
  }
  
  auto pcollid = j.getString("coll");
  if (!pcollid) {
    server->sendErr("no coll");
    return;
  }

  if (!SchemaImpl::existsGeneral(pcoll, pcollid.value())) {
    server->sendErr("collection does not exist to move to " + pcollid.value());
    return;
  }
  
  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no id");
    return;
  }

  auto me = j.getString("me");
  if (!me) {
    server->sendErr("no me");
    return;
  }
  
  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(objtype.value(), &coll, false)) {
    server->sendErr("Could not get collection name for moveObjectMsg");
    return;
  }
  
  auto result = SchemaImpl::findByIdGeneral(coll, id.value(), {});
  if (!result) {
    server->sendErr(objtype.value() + " can't find");
    return;
  }
  auto orig = result->value();
  if (!orig) {
    server->sendErr(objtype.value() + " not found");
    return;
  }
  
  L_TRACE(objtype.value() << " old value " << orig.value());
  
  auto origparent = orig.value().getString(parentfield);
  if (!origparent) {
    server->sendErr("parent field not found");
    return;
  }

  if (!Security::instance()->canEdit(pcoll, me, origparent.value())) {
    L_ERROR("no edit for " << objtype.value() << " " << id.value());
    server->sendSecurity();
    return;
  }
  
  // Set the parent field of the object.
  Data obj = {
    { parentfield, pcollid.value() }
  };

  obj.setObj("modifyDate", Storage::instance()->getNow());
  
  // send to other nodes.
  Data obj2 = obj;
  if (orig.value().has("upstream")) {
    obj2.setBool("upstream", orig.value().getBool("upstream").value());
  }
  server->sendMov(objtype.value(), id.value(), obj2, parenttype, origparent.value());
    
  // update locally
  L_TRACE("updating " << obj);
  auto r = SchemaImpl::updateGeneralById(coll, id.value(), {
    { "$set", obj }
  });
  if (!r) {
    server->sendErr("could not update " + objtype.value());
    return;
  }
  
  // and reply back
  L_TRACE("updated " << r.value());
  server->sendAck();

}

};