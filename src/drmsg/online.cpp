/*
  online.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 3-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"
#include "storage/schema.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void onlineMsg(Server *server, json &j) {
   
  BOOST_LOG_TRIVIAL(trace) << "online " << j;
       
  string src;
  if (!server->getSrc(j, &src)) {
    server->sendErrDown("online missing src");
    return;
  }
  
  auto pubKey = Json::getString(j, "pubKey");
  if (!pubKey) {
    server->sendErrDown("online missing pubKey");
    return;
  }
  
  boost::json::object obj;
  
  auto headerTitle = Json::getString(j, "headerTitle", true);
  if (headerTitle) {
    obj["headerTitle"] = headerTitle.value();
  }
  auto streamBgColor = Json::getString(j, "streamBgColor", true);
  if (streamBgColor) {
    obj["streamBgColor"] = streamBgColor.value();
  }
  auto mirror = Json::getString(j, "mirror", true);
  if (mirror) {
    obj["mirror"] = mirror.value();
  }
  auto synced = Json::getString(j, "synced", true);
  if (synced) {
    obj["synced"] = synced.value();
  }

  // is the node valid?
  bool valid = false;
  auto node = Node().find(json{ { "serverId", src } }, {}).value();
  if (node) {
    if (node.value().pubKey() == pubKey.value()) {
      obj["valid"] = true;
      Node().updateById(node.value().id(), obj);
      valid = true;
    }
  }
  else {
    obj["serverId"] = src;
    obj["pubKey"] = pubKey.value();
    
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

