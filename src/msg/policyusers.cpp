/*
  policyusersmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

void Server::policyUsersMsg(json &j) {

  auto policyid = Json::getString(j, "policy");
  if (!policyid) {
    sendErr("no policy");
    return;
  }

  vector<string> userids;
  Security::instance()->getPolicyUsers(policyid.value(), &userids);

  boost::json::array users;
  for (auto i: userids) {
    auto user = User().findById(i, { "id", "name", "fullname" }).value();
    if (user) {
      users.push_back(user.value().j());
    }
  }

  send({
    { "type", "policyusers" },
    { "id", policyid.value() },
    { "users", users }
  });
  
}
