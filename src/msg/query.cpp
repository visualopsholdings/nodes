/*
  query.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 15-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "upstream.hpp"
#include "log.hpp"
#include "data.hpp"

namespace nodes {

void queryMsg(Server *server, const IncomingMsg &in) {

  if (!in.objtype) {
    server->sendErr("require objtype");
    return;
  }
  
  if (!server->_online) {
    server->sendErr("not online with upstream");
    return;
  }
  string fieldname = in.objtype.value() == "user" ? "fullname" : "name";
  auto fieldval = Dict::getString(in.extra_fields.get(fieldname));
  if (!fieldval) {
    server->sendErr("require " + fieldname + " for " + in.objtype.value() + " query");
    return;
  }
  boost::json::object msg = {
    { "type", "query" },
    { "objtype", in.objtype.value() },
    { "fieldname", fieldname },
    { "fieldval", fieldval.value() }
  };
  server->setSrc(&msg);
  server->sendDataReq(in.corr, msg);
    
  server->sendAck();
  
}

};