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
#include "log.hpp"

namespace nodes {

void policyMsg(Server *server, const IncomingMsg &in) {

  auto objid = Dict::getString(in.extra_fields.get("id"));
  if (!objid) {
    server->sendErr("no object id");
    return;
  }
  if (!in.objtype) {
    server->sendErr("no object type");
    return;
  }

  // the collection names are pluralised object types.
  string collname;
  if (!Storage::instance()->collName(in.objtype.value(), &collname, false)) {
    server->sendErr("Could not get collection name for policy");
    return;
  }

  auto doc = Security::instance()->withView(collname, in.me, 
    dictO({{ { "_id", dictO({{ "$oid", objid.value() }}) } }}), { "policy" }).one();
  if (!doc) {
    server->sendErr("DB Error (no policy)");
    return;
  }
  
  // get the policy out of the JSON.
  auto policy = doc->getString("policy");

  auto lines = Security::instance()->getPolicyLines(policy);
  if (!lines) {
    server->sendErr("Policy lines error");
    return;
  }
  server->send(dictO({
    { "type", "policy" },
    { "policy", lines.value() }
  }));
  
}

};