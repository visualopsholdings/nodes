/*
  policymsg.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 9-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void policyMsg(Server *server, json &j) {

  auto objid = Json::getString(j, "id");
  if (!objid) {
    server->sendErr("no object id");
    return;
  }
  auto objtype = Json::getString(j, "objtype");
  if (!objtype) {
    server->sendErr("no object type");
    return;
  }

  string policy;
  if (objtype.value() == "stream") {
    Stream stream;
    auto doc = Security::instance()->withView(stream, Json::getString(j, "me"), {{ { "_id", { { "$oid", objid.value() } } } }}).value();
    if (!doc) {
      server->sendErr("DB Error");
      return;
    }
    policy = doc.value().policy();
  }
  else if (objtype.value() == "group") {
    Group group;
    auto doc = Security::instance()->withView(group, Json::getString(j, "me"), {{ { "_id", { { "$oid", objid.value() } } } }}).value();
    if (!doc) {
      server->sendErr("DB Error");
      return;
    }
    policy = doc.value().policy();
  }
  else {
    server->sendErr("unknown objtype " + objtype.value());
    return;
  }

  auto lines = Security::instance()->getPolicyLines(policy);
  if (!lines) {
    server->sendErr("Policy lines error");
    return;
  }
  server->send({
    { "type", "policy" },
    { "policy", lines.value() }
  });
  
}

};