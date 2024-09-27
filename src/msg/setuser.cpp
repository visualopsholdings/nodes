/*
  setuser.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void setuserMsg(Server *server, json &j) {

  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id in user");
    return;
  }  
  
  auto doc = User().findById(id.value(), {}).value();
  if (doc) {
    BOOST_LOG_TRIVIAL(trace) << "user old value " << doc.value().j();
    
    auto fullname = Json::getString(j, "fullname");
    auto name = Json::getString(j, "name");
    auto admin = Json::getBool(j, "admin");
    auto active = Json::getBool(j, "active");

    boost::json::object obj = {
      { "modifyDate", Storage::instance()->getNow() },
    };
    if (fullname) {
      obj["fullname"] = fullname.value();
    }    
    if (name) {
      obj["name"] = name.value();
    }    
    if (active) {
      obj["active"] = active.value();
    }    
    if (admin) {
      obj["admin"] = admin.value();
    }
    
    BOOST_LOG_TRIVIAL(trace) << "updating " << obj;
    auto result = User().updateById(id.value(), obj);
    if (!result) {
      server->sendErr("could not update user");
      return;
    }
    BOOST_LOG_TRIVIAL(trace) << "updated " << result.value();
    server->sendAck();
    return;
  }

  server->sendErr("could not find user " + id.value());
  
}

};