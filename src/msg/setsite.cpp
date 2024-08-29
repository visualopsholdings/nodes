/*
  setsite.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 14-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void setsiteMsg(Server *server, json &j) {

  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id in site");
    return;
  }  
  
  auto doc = Site().findById(id.value(), {}).value();
  if (doc) {
    BOOST_LOG_TRIVIAL(trace) << "site old value " << doc.value().j();
    
    boost::json::object obj;
    auto headerTitle = Json::getString(j, "headerTitle");
    auto streamBgColor = Json::getString(j, "streamBgColor");
    if (headerTitle) {
      obj["headerTitle"] = headerTitle.value();
    }
    if (streamBgColor) {
      obj["streamBgColor"] = streamBgColor.value();
    }
    BOOST_LOG_TRIVIAL(trace) << "updating " << obj;
    auto result = Site().updateById(id.value(), obj);
    if (!result) {
      server->sendErr("could not update site");
      return;
    }
    BOOST_LOG_TRIVIAL(trace) << "updated " << result.value();
    server->sendAck();
    return;
  }

  auto result = Site().insert({
    { "headerTitle", Json::getString(j, "headerTitle").value() },
    { "streamBgColor", Json::getString(j, "streamBgColor").value() }
  });
  if (!result) {
    server->sendErr("could not insert site");
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << "inserted " << result.value();
  server->sendAck();
  
}

};