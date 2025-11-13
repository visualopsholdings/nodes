/*
  online.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 3-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "storage/schema.hpp"
#include "log.hpp"

namespace nodes {

void onlineMsg(Server *server, const IncomingMsg &in) {
   
  string src;
  if (!server->getSrc(in, &src)) {
    server->sendErrDown("online missing src");
    return;
  }
  
  auto pubKey = Dict::getString(in.extra_fields.get("pubKey"));
  if (!pubKey) {
    server->sendErrDown("online missing pubKey");
    return;
  }
  
  DictO obj;
  
  auto headerTitle = Dict::getString(in.extra_fields.get("headerTitle"));
  if (headerTitle) {
    obj["headerTitle"] = *headerTitle;
  }
  auto streamBgColor = Dict::getString(in.extra_fields.get("streamBgColor"));
  if (streamBgColor) {
    obj["streamBgColor"] = *streamBgColor;
  }
  auto mirror = Dict::getString(in.extra_fields.get("mirror"));
  if (mirror) {
    obj["mirror"] = *mirror;
  }
  auto synced = Dict::getString(in.extra_fields.get("synced"));
  if (synced) {
    obj["synced"] = *synced;
  }

  // is the node valid?
  bool valid = false;
  auto node = Node().find(dictO({{ "serverId", src }}), {}).one();
  if (node) {
    if (node.value().pubKey() == pubKey.value()) {
      obj["valid"] = true;
      Node().updateById(node.value().id(), obj);
      valid = true;
    }
  }
  else {
    obj["serverId"] = src;
    obj["pubKey"] = *pubKey;
    
    // create a new node.
    auto result = Node().insert(obj);
    if (!result) {
      server->sendErrDown("online couldnt create node");
      return;
    }
  }
  
  server->sendDown(dictO({
    { "type", "upstream" },
    { "id", server->_serverId },
    { "valid", valid },
    { "dest", src }   
  }));
  
}

};

