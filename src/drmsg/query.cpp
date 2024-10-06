/*
  query.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 6-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"
#include "storage/schema.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void queryDrMsg(Server *server, json &j) {
   
  BOOST_LOG_TRIVIAL(trace) << "query " << j;
       
  auto src = Json::getString(j, "src");
  if (!src) {
    server->sendErrDown("query missing src");
    return;
  }
  
  auto corr = Json::getString(j, "corr");
  if (!corr) {
    server->sendErrDown("query missing corr");
    return;
  }
  
  auto userq = Json::getObject(j, "user", true);
  if (!userq) {
    server->sendErrDown("query only handles user queries");
    return;
  }

  auto email = Json::getString(userq.value(), "email");
  if (!email) {
    server->sendErrDown("user query didnt have email");
    return;
  }
  
  auto docs = User().find(json{ { "fullname", { { "$regex", email.value() }, { "$options", "i" } } } }, { "fullname" }).values();

  boost::json::array s;
  if (docs) {
    for (auto i: docs.value()) {
      s.push_back(i.j());
    }
  }
  server->sendDown({
    { "type", "queryResult" },
    { "queryType", "user" },
    { "result", s },
    { "corr", corr.value() },
    { "dest", src.value() }   
  });
  
}

};