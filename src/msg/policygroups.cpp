/*
  policygroupsmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 12-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include "log.hpp"

namespace nodes {

void policyGroupsMsg(Server *server, Data &j) {

  auto policyid = j.getString("policy");
  if (!policyid) {
    server->sendErr("no policy");
    return;
  }

  vector<string> groupids;
  Security::instance()->getPolicyGroups(policyid.value(), &groupids);

  boost::json::array groups;
  for (auto i: groupids) {
    auto group = Group().findById(i, { "id", "name" }).value();
    if (group) {
      groups.push_back(group.value().j());
    }
  }

  server->send({
    { "type", "policygroups" },
    { "id", policyid.value() },
    { "groups", groups }
  });
  
}

};