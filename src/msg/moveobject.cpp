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

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(parenttype, &coll)) {
    server->sendErr("Could not get collection name for move object");
    return;
  }
  
  auto collid = Json::getString(j, "coll");
  if (!collid) {
    server->sendErr("no coll");
    return;
  }

  if (!SchemaImpl::existsGeneral(coll, collid.value())) {
    server->sendErr("collection does not exist to move to " + collid.value());
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
  
  // Set the parent field of the object.
  boost::json::object obj = {
    { parentfield, collid.value() }
  };

  Handler::update(server, objtype.value(),  id.value(), 
    Json::getString(j, "me", true), nullopt, &obj);

}

};