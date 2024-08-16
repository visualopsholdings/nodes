/*
  upstream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"

#include <boost/log/trivial.hpp>

void Server::upstreamMsg(json &j) {
   
  auto msg = Json::getString(j, "msg");
  if (msg) {
    BOOST_LOG_TRIVIAL(error) << "online err " << msg.value();
    return;
  }
  
  auto type = Json::getString(j, "type");
  if (!type) {
    BOOST_LOG_TRIVIAL(error) << "reply missing type";
    return;
  }
  
  if (type.value() == "upstream") {
    auto id = Json::getString(j, "id");
    if (!id) {
      BOOST_LOG_TRIVIAL(error) << "got upstream with no id";
      return;
    }
    _upstreamId = id.value();
    _online = true;
    BOOST_LOG_TRIVIAL(trace) << "upstream " << _upstreamId;
    return;
  }
  BOOST_LOG_TRIVIAL(error) << "unknown msg type " << type.value();
     
}
