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
#include "dict.hpp"

using namespace vops;

namespace nodes {

void objectMsg(Server *server, const IncomingMsg &in) {

  if (!in.objtype) {
    server->sendErr("no object type");
    return;
  }
  auto id = Dict::getString(in.extra_fields.get("id"));
  if (!id) {
    server->sendErr("no id for " + *in.objtype);
    return;
  }

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(*in.objtype, &coll)) {
    server->sendErr("Could not get collection name for object");
    return;
  }
  
  auto doc = Security::instance()->withView(coll, in.me, 
    {{ { "_id", { { "$oid", *id } } } }}).one();
  if (!doc) {
    L_ERROR("no object to view");
    server->sendSecurity();
    return;
  }

  server->sendObject(in, *in.objtype, doc.value().d().dict());

}

};