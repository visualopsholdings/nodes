/*
  setstream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-Sep-2024
  
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

void setStreamMsg(Server *server, json &j) {

  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id in group");
    return;
  }  
  
  Stream streams;
  boost::json::object obj;
  auto streambits = Json::getNumber(j, "streambits");
  if (streambits) {
    obj["streambits"] = streambits.value();
  }
  Handler<StreamRow>::update(server, streams, "stream",  id.value(), 
    Json::getString(j, "me", true), Json::getString(j, "name", true), &obj);

}

};