/*
  sharelink.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 12-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include "log.hpp"

namespace nodes {

void shareLinkMsg(Server *server, json &j) {

  auto group = Json::getString(j, "group");
  if (!group) {
    server->sendErr("no group");
    return;
  }
  auto expires = Json::getNumber(j, "expires");
  if (!group) {
    server->sendErr("no expires");
    return;
  }
  auto me = Json::getString(j, "me");
  if (!me) {
    server->sendErr("no me");
    return;
  }
  auto coll = Json::getString(j, "collection");
  if (!coll) {
    server->sendErr("no collection");
    return;
  }
  auto id = Json::getString(j, "id");
  if (!id) {
    server->sendErr("no id");
    return;
  }
  auto bitsfield = Json::getString(j, "bitsfield");
  if (!bitsfield) {
    server->sendErr("no bitsfield");
    return;
  }
  auto urlpostfix = Json::getString(j, "urlpostfix");
  if (!urlpostfix) {
    server->sendErr("no urlpostfix");
    return;
  }

  auto doc = Security::instance()->withEdit(coll.value(), me, {{ { "_id", { { "$oid", id.value() } } } }}).value();
  if (!doc) {
    server->sendErr("DB Error (no " + coll.value() + ")");
    return;
  }
  // source this path...
  auto urlprefix = (server->_test ? "http://" : "https://") + server->_hostName + urlpostfix.value();
  auto url = Security::instance()->generateShareLink(me.value(), urlprefix, coll.value(), id.value(), group.value(), expires.value(), bitsfield.value());
  if (!url) {
    server->sendErr("couldn't create share link");
    return;
  }
  server->send({
    { "type", "sharelink" },
    { "url", url.value() }
  });

}

};