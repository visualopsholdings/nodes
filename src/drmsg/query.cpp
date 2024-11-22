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
   
  BOOST_LOG_TRIVIAL(trace) << "query (dr)" << j;
       
  string src;
  if (!server->getSrc(j, &src)) {
    server->sendErrDown("query missing src");
    return;
  }
  
  auto corr = Json::getString(j, "corr");
  if (!corr) {
    server->sendErrDown("query missing corr");
    return;
  }
  
  // calculate the type of query we want.
  string fieldname;
  string type;
  string val;
  
  auto userq = Json::getObject(j, "user", true);
  if (userq) {
    type = "user";
    fieldname = "fullname";
    auto v = Json::getString(userq.value(), "email");
    if (!v) {
      server->sendErrDown("user query didnt have email");
      return;
    }
    val = v.value();
  }
  else {
    auto groupq = Json::getObject(j, "group", true);
    if (groupq) {
      type = "group";
      fieldname = "name";
      auto v = Json::getString(groupq.value(), "name");
      if (!v) {
        server->sendErrDown("group query didnt have name");
        return;
      }
      val = v.value();
    }
    else {
      auto streamq = Json::getObject(j, "stream", true);
      if (streamq) {
        type = "stream";
        fieldname = "name";
        auto v = Json::getString(streamq.value(), "name");
        if (!v) {
          server->sendErrDown("stream query didnt have name");
          return;
        }
        val = v.value();
      }
      else {
        server->sendErrDown("query only handles user, group and stream queries");
        return;
      }
    }
  }
  
  auto result = SchemaImpl::findGeneral(type + "s", json{ { fieldname, { { "$regex", val }, { "$options", "i" } } } }, { fieldname });
  if (!result) {
    server->sendErrDown("find failed");
    return;
  }
      
  auto docs = result->values();
  server->sendDown({
    { "type", "queryResult" },
    { "queryType", type },
    { "result", docs ? docs.value() : boost::json::array() },
    { "corr", corr.value() },
    { "dest", src }   
  });
 
}

};