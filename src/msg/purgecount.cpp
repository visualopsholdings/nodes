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
  auto parent = Storage::instance()->parentField(objtype.value());
  if (parent) {
    auto pid = Json::getString(j, parent.value());
    if (!pid) {
      server->sendErr("no " + parent.value());
      return;
    }
    query[parent.value()] = pid.value();
  }
  
  // get the collection name.
  string coll = Storage::instance()->collName(objtype.value());
  
  server->send({
    { "type", "count" },
    { "count", SchemaImpl::countGeneral(coll, query) }
  });
  
}

};