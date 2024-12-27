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
#include "data.hpp"

namespace nodes {

void onlineMsg(Server *server, Data &j) {
   
  L_TRACE("online " << j);
       
  string src;
  if (!server->getSrc(j, &src)) {
    server->sendErrDown("online missing src");
    return;
  }
  
  auto pubKey = j.getString("pubKey");
  if (!pubKey) {
    server->sendErrDown("online missing pubKey");
    return;
  }
  
  Data obj = {{}};
  
  auto headerTitle = j.getString("headerTitle", true);
  if (headerTitle) {
    obj.setString("headerTitle", headerTitle.value());
  }
  auto streamBgColor = j.getString("streamBgColor", true);
  if (streamBgColor) {
    obj.setString("streamBgColor", streamBgColor.value());
  }
  auto mirror = j.getString("mirror", true);
  if (mirror) {
    obj.setString("mirror", mirror.value());
  }
  auto synced = j.getString("synced", true);
  if (synced) {
    obj.setString("synced", synced.value());
  }

  // is the node valid?
  bool valid = false;
  auto node = Node().find(json{ { "serverId", src } }, {}).value();
  if (node) {
    if (node.value().pubKey() == pubKey.value()) {
      obj.setBool("valid", true);
      Node().updateById(node.value().id(), obj);
      valid = true;
    }
  }
  else {
    obj.setString("serverId", src);
    obj.setString("pubKey", pubKey.value());
    
    // create a new node.
    auto result = Node().insert(obj);
    if (!result) {
      server->sendErrDown("online couldnt create node");
      return;
    }
  }
  
  server->sendDown({
    { "type", "upstream" },
    { "id", server->_serverId },
    { "valid", valid },
    { "dest", src }   
  });
  
}

};

