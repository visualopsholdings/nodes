/*
  members.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void membersMsg(Server *server, json &j) {

  auto groupid = Json::getString(j, "group");
  if (!groupid) {
    server->sendErr("no group");
    return;
  }

  auto doc = Group().find(json{ { "_id", { { "$oid", groupid.value() } } } }, { "members" }).value();

  if (!doc) {
    server->sendErr("can't find group " + groupid.value());
    return;
  }
  
  boost::json::array newmembers;
  auto members = doc.value().j().at("members").as_array();
  for (auto i: members) {
    boost::json::object newmember = i.as_object();
    auto user = User().findById(i.at("user").as_string().c_str(), { "fullname" }).value();
    if (user) {
      newmember["fullname"] = user.value().fullname();
    }
    newmembers.push_back(newmember);
  }
  
  server->send({
    { "type", "members" },
    { "members", newmembers }
  });
  
}

};