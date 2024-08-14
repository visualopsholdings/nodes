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

void Server::setsiteMsg(json &j) {

  auto id = Json::getString(j, "id");
  if (!id) {
    sendErr("no id in site");
    return;
  }  
  
  auto doc = Site().findById(id.value(), {}).value();
  if (doc) {
    BOOST_LOG_TRIVIAL(trace) << "site old value " << doc.value().j();
    json obj = {{ "$set", {{ "headerTitle", Json::getString(j, "headerTitle").value() }, { "streamBgColor", Json::getString(j, "streamBgColor").value() }} }};
    BOOST_LOG_TRIVIAL(trace) << "updating " << obj;
    auto result = Site().updateById(id.value(), obj);
    if (!result) {
      sendErr("could not update site");
      return;
    }
    BOOST_LOG_TRIVIAL(trace) << "updated " << result.value();
    sendAck();
    return;
  }

  auto result = Site().insert({
    { "headerTitle", Json::getString(j, "headerTitle").value() },
    { "streamBgColor", Json::getString(j, "streamBgColor").value() }
  });
  if (!result) {
    sendErr("could not insert site");
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << "inserted " << result.value();
  sendAck();
  
}

