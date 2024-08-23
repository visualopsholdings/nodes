/*
  newmember.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/join.hpp>
#include <set>

namespace nodes {

void newMemberMsg(Server *server, json &j) {

  auto groupid = Json::getString(j, "group");
  if (!groupid) {
    server->sendErr("no group");
    return;
  }

  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id");
    return;
  }

  auto doc = Group().findById(groupid.value(), { "members" }).value();
  if (!doc) {
    server->sendErr("can't find group " + groupid.value());
    return;
  }
  auto members = doc.value().j().at("members").as_array();
  
//  BOOST_LOG_TRIVIAL(trace) << "old members " << members;

  set<string> mset;
  for (auto i: members) {
    mset.insert(i.at("user").as_string().c_str());
  }
  mset.insert(id.value());

//  BOOST_LOG_TRIVIAL(trace) << "new set " << boost::algorithm::join(mset, ", ");

  boost::json::array newmembers;
  for (auto i: mset) {
    newmembers.push_back({
      { "user", i }
    });
  }
  
  auto result = Group().updateById(groupid.value() , {
    { "members", newmembers }
  });
  if (!result) {
    server->sendErr("could not update group");
    return;
  }
  server->sendAck();

}

};