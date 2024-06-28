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

#include <boost/log/trivial.hpp>

void Server::policyUsersMsg(json &j, shared_ptr<Storage> storage) {

  string policyid;
  if (!getString(j, "policy", &policyid)) {
    sendErr("no policy");
    return;
  }

  vector<string> userids;
  Security::instance()->getPolicyUsers(*storage, policyid, &userids);

  boost::json::array users;
  for (auto i: userids) {
    auto user = User(*storage).findById(i, { "id", "name", "fullname" }).value();
    if (user) {
      users.push_back(user.value());
    }
  }

  send({
    { "type", "policyusers" },
    { "id", policyid },
    { "users", users }
  });
  
}
