/*
  purgecount.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "security.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void purgeCountMsg(Server *server, json &j) {

  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }
  
  boost::json::object query = { 
    { "deleted", true } 
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
    server->sendErr("Could not get collection name for purgecount");
    return;
  }
  
  server->send({
    { "type", "count" },
    { "count", SchemaImpl::countGeneral(coll, query) }
  });
  
}

};