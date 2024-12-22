/*
  objectsmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include "log.hpp"

namespace nodes {

void objectsMsg(Server *server, json &j) {

  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }
  boost::json::object query = {
    { "deleted", {
      { "$ne", true }
      }
    }
  };
  
  // add in any parent query.
  string parent;
  if (Storage::instance()->parentInfo(objtype.value(), &parent)) {
    auto pid = Json::getString(j, parent);
    if (!pid) {
      server->sendErr("no " + parent);
      return;
    }
    query[parent] = pid.value();
  }
  
  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(objtype.value(), &coll)) {
    server->sendErr("Could not get collection name for objects");
    return;
  }
  
  // make sure it can be viewed.
  auto docs = Security::instance()->withView(coll, Json::getString(j, "me", true), query).values();
  
  // copy out all the data to return;
  boost::json::array s;
  if (docs) {
    transform(docs.value().begin(), docs.value().end(), back_inserter(s), [](auto e) { return e.j(); });
  }
  
  server->sendCollection(j, coll, s);

}

};