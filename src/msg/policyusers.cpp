/*
  policyusersmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void policyUsersMsg(Server *server, const IncomingMsg &in) {

  auto policyid = Dict::getString(in.extra_fields.get("policy"));
  if (!policyid) {
    server->sendErr("no policy");
    return;
  }

  vector<string> userids;
  Security::instance()->getPolicyUsers(policyid.value(), &userids);

  DictV users;
  for (auto i: userids) {
    auto user = User().findById(i, { "id", "name", "fullname" }).one();
    if (user) {
      users.push_back(user->dict());
    }
  }

  server->send(dictO({
    { "type", "policyusers" },
    { "id", policyid.value() },
    { "users", users }
  }));
  
}

};