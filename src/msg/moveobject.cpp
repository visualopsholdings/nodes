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
#include "json.hpp"
#include "handler.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void moveObjectMsg(Server *server, json &j) {

  auto objtype = Json::getString(j, "objtype");
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
  
  auto pcollid = Json::getString(j, "coll");
  if (!pcollid) {
    server->sendErr("no coll");
    return;
  }

  if (!SchemaImpl::existsGeneral(pcoll, pcollid.value())) {
    server->sendErr("collection does not exist to move to " + pcollid.value());
    return;
  }
  
  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id");
    return;
  }

  auto me = Json::getString(j, "me");
  if (!me) {
    server->sendErr("no me");
    return;
  }
  
  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(objtype.value(), &coll, false)) {
    server->sendErr("Could not get collection name for Handler::update");
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
  
  BOOST_LOG_TRIVIAL(trace) << objtype.value() << " old value " << orig.value();
  
  auto origparent = Json::getString(orig.value(), parentfield);
  if (!origparent) {
    server->sendErr("parent field not found");
    return;
  }

  if (!Security::instance()->canEdit(pcoll, me, origparent.value())) {
    BOOST_LOG_TRIVIAL(error) << "no edit for " << objtype.value() << " " << id.value();
    server->sendSecurity();
    return;
  }
  
  // Set the parent field of the object.
  boost::json::object obj = {
    { parentfield, pcollid.value() }
  };

  obj["modifyDate"] = Storage::instance()->getNow();
  
  // send to other nodes.
  boost::json::object obj2 = obj;
  if (orig.value().as_object().if_contains("upstream")) {
    obj2["upstream"] = Json::getBool(orig.value(), "upstream").value();
  }
  server->sendMov(objtype.value(), id.value(), obj2, parenttype, origparent.value());
    
  // update locally
  BOOST_LOG_TRIVIAL(trace) << "updating " << obj;
  auto r = SchemaImpl::updateGeneralById(coll, id.value(), {
    { "$set", obj }
  });
  if (!r) {
    server->sendErr("could not update " + objtype.value());
    return;
  }
  
  // and reply back
  BOOST_LOG_TRIVIAL(trace) << "updated " << r.value();
  server->sendAck();

}

};