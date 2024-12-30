/*
  object.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void objectMsg(Server *server, Data &j) {

  auto objtype = j.getString("objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }
  auto id = j.getString("id");
  if (!id) {
    server->sendErr("no " + objtype.value());
    return;
  }

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(objtype.value(), &coll)) {
    server->sendErr("Could not get collection name for object");
    return;
  }
  
  auto doc = Security::instance()->withView(coll, j.getString("me", true), 
    {{ { "_id", { { "$oid", id.value() } } } }}).value();
  if (!doc) {
    L_ERROR("no object to view");
    server->sendSecurity();
    return;
  }

  server->sendObject(j, objtype.value(), doc.value().d());

}

};