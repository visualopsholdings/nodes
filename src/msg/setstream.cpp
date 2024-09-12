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

#include <boost/log/trivial.hpp>

namespace nodes {

void setStreamMsg(Server *server, json &j) {

  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id in stream");
    return;
  }  
  
  Stream streams;
  if (!Security::instance()->canEdit(streams, Json::getString(j, "me"), id.value())) {
    server->sendErr("can't edit streams " + id.value());
    return;
  }

  auto doc = streams.findById(id.value(), {}).value();
  if (!doc) {
    server->sendErr("stream not found");
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << "stream old value " << doc.value().j();
  
  boost::json::object obj;
  auto name = Json::getString(j, "name");
  auto streambits = Json::getNumber(j, "streambits");
  if (name) {
    obj["name"] = name.value();
  }
  if (streambits) {
    obj["streambits"] = streambits.value();
  }
  BOOST_LOG_TRIVIAL(trace) << "updating " << obj;
  auto result = streams.updateById(id.value(), obj);
  if (!result) {
    server->sendErr("could not update stream");
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << "updated " << result.value();
  server->sendAck();

}

};