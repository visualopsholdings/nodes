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

  // the collection names are pluralised object types.
  string collname = objtype.value() + "s";
  auto doc = Security::instance()->withView(collname, Json::getString(j, "me", true), 
    {{ { "_id", { { "$oid", objid.value() } } } }}, { "policy" }).value();
  if (!doc) {
    server->sendErr("DB Error (no policy)");
    return;
  }
  
  // get the policy out of the JSON.
  string policy = boost::json::value_to<string>(doc.value().j().at("policy"));

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