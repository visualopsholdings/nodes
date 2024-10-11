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
  auto lastGroup = Json::getString(j, "lastGroup", true);
  auto groups = Json::getArray(j, "groups", true);
  if (lastGroup) {
    obj["lastGroup"] = lastGroup.value();
  }
  if (groups) {
    obj["groups"] = groups.value();
  }
  auto lastStream = Json::getString(j, "lastStream", true);
  auto streams = Json::getArray(j, "streams", true);
  if (lastStream) {
    obj["lastStream"] = lastStream.value();
  }
  if (streams) {
    obj["streams"] = streams.value();
  }
  Node().updateById(node.value().id(), obj);
  		
  // here is where we would send on everything
  boost::json::array msgs;
  boost::json::array sendusers;
  boost::json::array sendgroups;
  boost::json::array sendstreams;
  boost::json::array sendideas;

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
  
  if (lastGroup && groups && groups.value().size() > 0) {
  
    auto q = SchemaImpl::idRangeAfterDateQuery(groups.value(), lastGroup.value());
  
    auto results = SchemaImpl::findGeneral("groups", q, {});
    if (results) {
      auto groups = results->values();
      if (groups) {
        for (auto u: groups.value()) {
          sendgroups.push_back(u);
        }
      }
    }
  }
  
  if (lastStream && streams && streams.value().size() > 0) {
  
    auto q = SchemaImpl::idRangeAfterDateQuery(streams.value(), lastStream.value());
  
    // find all the streams that are newer.
    auto results = SchemaImpl::findGeneral("streams", q, {});
    if (results) {
      auto streams = results->values();
      if (streams) {
        for (auto s: streams.value()) {
          sendstreams.push_back(s);
        }
      }
    }
    
    // find all ideas for the streams that are newer.
    for (auto s: streams.value()) {
      BOOST_LOG_TRIVIAL(trace) << "ideas for " << s;
      BOOST_LOG_TRIVIAL(trace) << "after " << lastStream.value();
      auto iq = SchemaImpl::streamAfterDateQuery(s.as_string().c_str(), lastStream.value());
      auto idear = SchemaImpl::findGeneral("ideas", iq, {});
      if (idear) {
        auto ideas = idear->values();
        if (ideas) {
          for (auto i: ideas.value()) {
            sendideas.push_back(i);
          }
        }
      }
    }
  }
  
  msgs.push_back({
    { "type", "user" },
    { "objs", sendusers }
  });
  msgs.push_back({
    { "type", "group" },
    { "objs", sendgroups }
  });
  msgs.push_back({
    { "type", "stream" },
    { "objs", sendstreams }
  });
  msgs.push_back({
    { "type", "idea" },
    { "objs", sendideas }
  });
  
  server->sendDown({
    { "type", "discoverResult" },
    { "msgs", msgs },
    
  });
  
}

};
