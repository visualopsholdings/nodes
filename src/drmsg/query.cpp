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

void queryDrMsg(Server *server, const IncomingMsg &in) {
   
  string src;
  if (!server->getSrc(in, &src)) {
    server->sendErrDown("query missing src");
    return;
  }
  
  auto corr = Dict::getString(in.extra_fields.get("corr"));
  if (!corr) {
    server->sendErrDown("query missing corr");
    return;
  }
  
  if (!in.objtype) {
    server->sendErrDown("query missing objtype");
    return;
  }
  
  auto fieldname = Dict::getString(in.extra_fields.get("fieldname"));
  if (!fieldname) {
    server->sendErrDown("query missing fieldname");
    return;
  }
  
  auto fieldval = Dict::getString(in.extra_fields.get("fieldval"));
  if (!fieldval) {
    server->sendErrDown("query missing fieldval");
    return;
  }

  // get the collection name.
  string coll;
  if (!Storage::instance()->collName(in.objtype.value(), &coll, false)) {
    server->sendErr("Could not get collection name for query");
    return;
  }

  auto result = SchemaImpl::findGeneral(coll, dictO({{  
    fieldname.value(), dictO({ { "$regex", fieldval.value() }, { "$options", "i" } }) 
  }}), { fieldname.value() });
  if (!result) {
    server->sendErrDown("find failed");
    return;
  }
      
  auto docs = result->all();
  DictV empty;
  server->sendDown(dictO({
    { "type", "queryResult" },
    { "objtype", in.objtype.value() },
    { "result", docs ? docs.value() : empty },
    { "corr", corr.value() },
    { "dest", src }   
  }));
 
}

};