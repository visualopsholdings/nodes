/*
  setobject.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Nov-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"
#include "handler.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void setObjectMsg(Server *server, json &j) {

  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }
  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id in set object");
    return;
  }  
  
//  BOOST_LOG_TRIVIAL(trace) << j;
  
  // set on all fields passed in except name.
  boost::json::object obj = j.as_object();
  obj.erase("type");
  obj.erase("objtype");
  obj.erase("me");
  obj.erase("id");
  obj.erase("name");

  Handler::update(server, objtype.value(),  id.value(), 
    Json::getString(j, "me", true), Json::getString(j, "name", true), &obj);

}

};