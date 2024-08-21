/*
  discoverResult.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void discoverResultMsg(Server *server, json &j) {
   
  auto msgs = Json::getArray(j, "msgs");
  if (!msgs) {
    BOOST_LOG_TRIVIAL(error) << "discoverResult missing msgs";
    return;
  }
  for (auto m: msgs.value()) {
    auto type = Json::getString(m, "type");
    if (!type) {
      BOOST_LOG_TRIVIAL(error) << "msg missing type";
      continue;
    }
    if (type.value() == "user") {
      auto objs = Json::getArray(m, "objs");
      if (!objs) {
        BOOST_LOG_TRIVIAL(error) << "msg missing objs";
        continue;
      }
      BOOST_LOG_TRIVIAL(trace) << "import users " << objs.value();
    }
    else {
      BOOST_LOG_TRIVIAL(error) << "only user msgs supported";
      continue;
    }
  }
   
}

};