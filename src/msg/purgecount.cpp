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

void purgeCountMsg(Server *server, const IncomingMsg &in) {

  if (!in.objtype) {
    server->sendErr("no object type");
    return;
  }
  
  auto query = dictO({ 
    { "deleted", true } 
  });
  
  // add in any parent query.
  string parent;
  if (Storage::instance()->parentInfo(in.objtype.value(), &parent)) {
    auto pid = Dict::getString(in.extra_fields.get(parent));
    if (!pid) {
      server->sendErr("no " + parent);
      return;
    }
    query[parent] = *pid;
  }  
  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(in.objtype.value(), &coll)) {
    server->sendErr("Could not get collection name for purgecount");
    return;
  }
  
  server->send(dictO({
    { "type", "count" },
    { "count", SchemaImpl::countGeneral(coll, query) }
  }));
  
}

};