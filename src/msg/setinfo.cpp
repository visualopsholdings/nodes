/*
  setinfo.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 8-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "json.hpp"

#include <boost/log/trivial.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp> 

void Server::setinfoMsg(json &j) {

  auto serverId = Json::getString(j, "serverId");
  if (serverId) {
    if (serverId.value() == "none") {
      BOOST_LOG_TRIVIAL(trace) << "reset server";
      if (resetServer()) {
        sendAck();
        connectUpstream();
      }
      else {
        sendErr("could not reset server");
      }
      return;
    }
    return;
  }

  stringstream ss;
  ss << "unknown " << j;
  sendErr(ss.str());

}

bool Server::setInfo(const string &name, const string &text) {

  auto doc = Info().find({{ "type", name }}, {"text"}).value();
  if (doc) {
    BOOST_LOG_TRIVIAL(trace) << "info old value " << doc.value().j();
    auto result = Info().update({{ "type", name }}, {{ "$set", {{ "text", text }} }});
    if (!result) {
      BOOST_LOG_TRIVIAL(error) << "could not update info";
      return false;
    }
    BOOST_LOG_TRIVIAL(trace) << "updated " << result.value();
    return true;
  }

  auto result = Info().insert({
    { "type", name },
    { "text", text }
  });
  if (!result) {
    BOOST_LOG_TRIVIAL(error) << "could not insert info";
    return false;
  }
  BOOST_LOG_TRIVIAL(trace) << "inserted " << result.value();
  return true;
  
}

#define defaultUpstream "nodes.visualops.com"
#define defaultUpstreamPubKey "K]<n72U1y#9PUS.j9BpC=XNxz6HCqhRfnGbSnajO"

bool Server::resetServer() {
  
  char pubkey[41];
  char seckey[41];
  int rc = zmq_curve_keypair(pubkey, seckey);
  if (rc != 0) {
    BOOST_LOG_TRIVIAL(error) << "failed to generate key pair " << rc;
    return false;
  }
  
  if (!setInfo("privateKey", seckey)) {
    return false;
  }
  if (!setInfo("pubKey", pubkey)) {
    return false;
  }
  if (!setInfo("upstream", defaultUpstream)) {
    return false;
  }
  if (!setInfo("upstreamPubKey", defaultUpstreamPubKey)) {
    return false;
  }
  
  // clear out all these flags.
  Info().deleteMany({{ "type", { { "$in", {"serverId", "upstreamMirror", "hasInitialSync", "upstreamLastSeen"}}} }});
  
  // generate a new server ID.
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  stringstream ss;
  ss << uuid;
  if (!setInfo("serverId", ss.str())) {
    return false;
  }
  
  return true;
}
