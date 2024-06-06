/*
  policyusersmsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"

#include <boost/log/trivial.hpp>

void Server::policyUsersMsg(json &j, shared_ptr<Storage> storage) {

  string policyid;
  if (!getString(j, "policy", &policyid)) {
    sendErr("no policy");
    return;
  }

  auto policy = Policy(*storage).find({ { "_id", { { "$oid", policyid } } } }, { "users" }).value();
  if (!policy) {
    sendErr("DB Error");
    return;
  }
  
  BOOST_LOG_TRIVIAL(trace) << policy.value();
  vector<string> userids;
  if (!getArray(policy, "users", &userids)) {
    sendErr("Users not found");
    return;
  }
  boost::json::array users;
  for (auto i: userids) {
    auto user = User(*storage).find({ { "_id", { { "$oid", i } } } }, { "_id", "name", "fullname" }).value();
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
