/*
  query.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 15-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"
#include "upstream.hpp"

#include "log.hpp"

namespace nodes {

void queryMsg(Server *server, json &j) {

  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErr("require objtype");
    return;
  }
  
  if (!server->_online) {
    server->sendErr("not online with upstream");
    return;
  }
  string fieldname = objtype.value() == "user" ? "fullname" : "name";
  auto fieldval = Json::getString(j, fieldname);
  if (!fieldval) {
    server->sendErr("require " + fieldname + " for " + objtype.value() + " query");
    return;
  }
  boost::json::object msg = {
    { "type", "query" },
    { "objtype", objtype.value() },
    { "fieldname", fieldname },
    { "fieldval", fieldval.value() }
  };
  server->setSrc(&msg);
  server->sendDataReq(Json::getString(j, "corr", true), msg);
    
  server->sendAck();
  
}

};