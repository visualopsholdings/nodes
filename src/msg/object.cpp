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
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void objectMsg(Server *server, json &j) {

  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }
  auto id = Json::getString(j, "id");
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
  
  auto doc = Security::instance()->withView(coll, Json::getString(j, "me", true), 
    {{ { "_id", { { "$oid", id.value() } } } }}).value();
  if (!doc) {
    BOOST_LOG_TRIVIAL(error) << "no object to view";
    server->sendSecurity();
    return;
  }

  server->sendObject(j, objtype.value(), doc.value().j());

}

};