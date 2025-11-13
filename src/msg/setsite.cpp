/*
  setsite.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 14-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"

namespace nodes {

void setsiteMsg(Server *server, const IncomingMsg &in) {

  if (!in.id) {
    server->sendErr("no id in site");
    return;
  }  

  auto obj = dictO({
    { "modifyDate", Storage::instance()->getNow() }
  });
  auto headerTitle = Dict::getString(in.extra_fields.get("headerTitle"));
  if (headerTitle) {
    obj["headerTitle"] = headerTitle.value();
  }
  auto streamBgColor = Dict::getString(in.extra_fields.get("streamBgColor"));
  if (streamBgColor) {
    obj["streamBgColor"] = streamBgColor.value();
  }
  
  auto doc = Site().findById(*in.id, {}).one();
  if (doc) {
    L_TRACE("site old value " << Dict::toString(doc->dict()));
    
    L_TRACE("updating " << Dict::toString(obj));
    auto result = Site().updateById(*in.id, obj);
    if (!result) {
      server->sendErr("could not update site");
      return;
    }
    L_TRACE("updated " << result.value());
    server->sendAck();
    return;
  }

  auto result = Site().insert(obj);
  if (!result) {
    server->sendErr("could not insert site");
    return;
  }
  
  L_TRACE("inserted " << Dict::toString(result.value()));
  server->sendAck();
  
}

};