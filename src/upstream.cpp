/*
  upstream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 13-Aug-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "upstream.hpp"

#include "server.hpp"

#include <boost/log/trivial.hpp>

Upstream::Upstream(Server *_server, zmq::context_t &context, int type, const string &name, const string &upstream, int port, const string &upstreamPubKey, 
    const string &privateKey, const string &pubKey) : 
      zmq::socket_t(context, type) {

#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)  
  setsockopt(ZMQ_CURVE_SERVERKEY, upstreamPubKey.c_str(), upstreamPubKey.size());
  setsockopt(ZMQ_CURVE_SECRETKEY, privateKey.c_str(), privateKey.size());
  setsockopt(ZMQ_CURVE_PUBLICKEY, pubKey.c_str(), pubKey.size());
#else
  _sub->set(zmq::sockopt::curve_serverkey, upstreamPubKey.c_str(), upstreamPubKey.size());
  _sub->set(zmq::sockopt::curve_secretkey, privateKey.c_str(), privateKey.size());
  _sub->set(zmq::sockopt::curve_publickey, pubKey.c_str(), pubKey.size());
#endif

  string url = "tcp://" + upstream + ":" + to_string(port);
	BOOST_LOG_TRIVIAL(trace) << "connecting to ZMQ " << url;
  connect(url);
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ " << name << " at "<< url;
	
  _monitor.reset(new Monitor(_server, name));
  _monitor->init(*this, "inproc://" + name, ZMQ_EVENT_CONNECTED + ZMQ_EVENT_DISCONNECTED);

}

void Upstream::check() {

  _monitor->check_event(20);

}

void Monitor::on_event_connected(const zmq_event_t& event, const char* addr) {
  BOOST_LOG_TRIVIAL(info) << "connected to " << _name;
  _connected = true;
  
  if (_name == "dataReq") {
    _server->online();
  }
  
}   

void Monitor::on_event_disconnected(const zmq_event_t& event, const char* addr) {
  BOOST_LOG_TRIVIAL(warning) << "disconnected from " << _name;
  _connected = false;
}
