/*
  query.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 6-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "storage/schema.hpp"
#include "storage.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void queryDrMsg(Server *server, Data &j) {
   
  L_TRACE("query (dr)" << j);
       
  string src;
  if (!server->getSrc(j, &src)) {
    server->sendErrDown("query missing src");
    return;
  }
  
  auto corr = j.getString("corr");
  if (!corr) {
    server->sendErrDown("query missing corr");
    return;
  }
  
  auto objtype = j.getString("objtype");
  if (!objtype) {
    server->sendErrDown("query missing objtype");
    return;
  }
  
  auto fieldname = j.getString("fieldname");
  if (!fieldname) {
    server->sendErrDown("query missing fieldname");
    return;
  }
  
  auto fieldval = j.getString("fieldval");
  if (!fieldval) {
    server->sendErrDown("query missing fieldval");
    return;
  }

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(objtype.value(), &coll, false)) {
    server->sendErr("Could not get collection name for query");
    return;
  }

  auto result = SchemaImpl::findGeneral(coll, { { fieldname.value(), 
    { { "$regex", fieldval.value() }, { "$options", "i" } } } }, { fieldname.value() });
  if (!result) {
    server->sendErrDown("find failed");
    return;
  }
      
  auto docs = result->values();
  Data empty;
  server->sendDown({
    { "type", "queryResult" },
    { "objtype", objtype.value() },
    { "result", docs ? docs.value() : empty },
    { "corr", corr.value() },
    { "dest", src }   
  });
 
}

};