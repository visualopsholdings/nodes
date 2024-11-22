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

#include <boost/log/trivial.hpp>

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
 
  string field;
  if (objtype.value() == "user") {
    field = "email";
  }
  else if (objtype.value() == "group" || objtype.value() == "stream") {
    field = "name";
  }
  else {
    server->sendErr("only user, group and stream queries available");
    return;
  }
  
  auto fieldval = Json::getString(j, field);
  if (!fieldval) {
    server->sendErr("require " + field + " for " + objtype.value() + " query");
    return;
  }
  json obj = {
    { field, fieldval.value() }
  };
  boost::json::object msg = {
    { "type", "query" },
    {  objtype.value(), obj }
  };
  server->setSrc(&msg);
  server->sendDataReq(Json::getString(j, "corr", true), msg);
    
  server->sendAck();
  
}

};