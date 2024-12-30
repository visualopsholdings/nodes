/*
  purgecount.cpp
  
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

void purgeCountMsg(Server *server, Data &j) {

  auto objtype = j.getString("objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }
  
  Data query = { 
    { "deleted", true } 
  };
  
  // add in any parent query.
  string parent;
  if (Storage::instance()->parentInfo(objtype.value(), &parent)) {
    auto pid = j.getString(parent);
    if (!pid) {
      server->sendErr("no " + parent);
      return;
    }
    query.setString(parent, pid.value());
  }
  
  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(objtype.value(), &coll)) {
    server->sendErr("Could not get collection name for purgecount");
    return;
  }
  
  server->send({
    { "type", "count" },
    { "count", SchemaImpl::countGeneral(coll, query) }
  });
  
}

};