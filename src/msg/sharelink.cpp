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
#include "log.hpp"

namespace nodes {

void shareLinkMsg(Server *server, const IncomingMsg &in) {

  auto group = Dict::getString(in.extra_fields.get("group"));
  if (!group) {
    server->sendErr("no group");
    return;
  }
  auto expires = Dict::getNum(in.extra_fields.get("expires"));
  if (!expires) {
    server->sendErr("no expires");
    return;
  }
  if (!in.me) {
    server->sendErr("no me");
    return;
  }
  auto coll = Dict::getString(in.extra_fields.get("collection"));
  if (!coll) {
    server->sendErr("no collection");
    return;
  }
  if (!in.id) {
    server->sendErr("no id");
    return;
  }
  auto bitsfield = Dict::getString(in.extra_fields.get("bitsfield"));
  if (!bitsfield) {
    server->sendErr("no bitsfield");
    return;
  }
  auto urlpostfix = Dict::getString(in.extra_fields.get("urlpostfix"));
  if (!urlpostfix) {
    server->sendErr("no urlpostfix");
    return;
  }

  auto doc = Security::instance()->withEdit(coll.value(), in.me, dictO({{ "_id", dictO({{ "$oid", in.id.value() }}) }})).one();
  if (!doc) {
    server->sendErr("DB Error (no " + coll.value() + ")");
    return;
  }
  // source this path...
  auto urlprefix = (server->_test ? "http://" : "https://") + server->_hostName + urlpostfix.value();
  auto url = Security::instance()->generateShareLink(in.me.value(), urlprefix, coll.value(), in.id.value(), group.value(), expires.value(), bitsfield.value());
  if (!url) {
    server->sendErr("couldn't create share link");
    return;
  }
  server->send(dictO({
    { "type", "sharelink" },
    { "url", url.value() }
  }));

}

};