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

  string policy;
  if (!getString(j, "policy", &policy)) {
    sendErr("no policy");
    return;
  }
  if (policy != "p1") {
    sendErr("not correct policy");
    return;
  }
  send({
    { "type", "policyusers" },
    { "id", "p1" },
    { "users", {
      { { "id", "u1" }, { "name", "tracy" }, { "fullname", "Tracy" } },
      { { "id", "u2" }, { "name", "leanne" }, { "fullname", "Leanne" } }  
      } 
    }
  });
  
}
