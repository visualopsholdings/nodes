/*
  streamsharelink.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 12-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void streamShareLinkMsg(Server *server, json &j) {

  auto stream = Json::getString(j, "stream");
  if (!stream) {
    server->sendErr("no stream");
    return;
  }
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

  Stream streams;
  auto doc = Security::instance()->withEdit(streams, Json::getString(j, "me"), {{ { "_id", { { "$oid", stream.value() } } } }}).value();
  if (!doc) {
    server->sendErr("DB Error");
    return;
  }
  auto url = Security::instance()->generateShareLink(stream.value(), group, expires);
  if (!url) {
    server->sendErr("couldn't create share link");
    return;
  }
  server->send({
    { "type", "streamsharelink" },
    { "url", url.value() }
  });

}

};