/*
  query.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 6-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"
#include "storage/schema.hpp"
#include "storage.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void queryDrMsg(Server *server, json &j) {
   
  BOOST_LOG_TRIVIAL(trace) << "query (dr)" << j;
       
  string src;
  if (!server->getSrc(j, &src)) {
    server->sendErrDown("query missing src");
    return;
  }
  
  auto corr = Json::getString(j, "corr");
  if (!corr) {
    server->sendErrDown("query missing corr");
    return;
  }
  
  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErrDown("query missing objtype");
    return;
  }
  
  auto fieldname = Json::getString(j, "fieldname");
  if (!fieldname) {
    server->sendErrDown("query missing fieldname");
    return;
  }
  
  auto fieldval = Json::getString(j, "fieldval");
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

  auto result = SchemaImpl::findGeneral(coll, json{ { fieldname.value(), 
    { { "$regex", fieldval.value() }, { "$options", "i" } } } }, { fieldname.value() });
  if (!result) {
    server->sendErrDown("find failed");
    return;
  }
      
  auto docs = result->values();
  server->sendDown({
    { "type", "queryResult" },
    { "objtype", objtype.value() },
    { "result", docs ? docs.value() : boost::json::array() },
    { "corr", corr.value() },
    { "dest", src }   
  });
 
}

};