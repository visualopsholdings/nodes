/*
  discover.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 3-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"
#include "storage/schema.hpp"
#include "date.hpp"

#include <boost/log/trivial.hpp>

using namespace bsoncxx::builder::basic;

namespace nodes {

void discoverMsg(Server *server, json &j) {
   
  BOOST_LOG_TRIVIAL(trace) << "discover " << j;
         
  auto src = Json::getString(j, "src");
  if (!src) {
    server->sendErrDown("discover missing src");
    return;
  }

  auto node = Node().find(json{ { "serverId", src.value() } }, {}).value();
  if (!node) {
    server->sendErrDown("discover no node");
    return;
  }
  
  if (!node.value().valid()) {
    server->sendErrDown("discover node invalid");
    return;
  }

  boost::json::object obj;
  auto lastUser = Json::getString(j, "lastUser", true);
  auto users = Json::getArray(j, "users", true);
  if (lastUser) {
    obj["lastUser"] = lastUser.value();
  }
  if (users) {
    obj["users"] = users.value();
  }
  Node().updateById(node.value().id(), obj);
  		
  // here is where we would send on everything
  boost::json::array msgs;
  boost::json::array sendusers;

  if (lastUser && users && users.value().size() > 0) {
  
    auto q = SchemaImpl::idRangeAfterDateQuery(users.value(), lastUser.value());
  
    auto results = SchemaImpl::findGeneral("users", q, {});
    if (results) {
      auto users = results->values();
      if (users) {
        for (auto u: users.value()) {
          sendusers.push_back(u);
        }
      }
    }
  }
  
  msgs.push_back({
    { "type", "user" },
    { "objs", sendusers }
  });
  
  server->sendDown({
    { "type", "discoverResult" },
    { "msgs", msgs },
    
  });
  
}

};
