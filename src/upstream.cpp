/*
  upstream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 13-Aug-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "upstream.hpp"

#include "server.hpp"

#include "log.hpp"

Upstream::Upstream(Server *_server, zmq::context_t &context, int type, const string &name, const string &upstream, int port, const string &upstreamPubKey, 
    const string &privateKey, const string &pubKey) : 
      _socket(context, type) {

	L_TRACE("setting curve options");
  int curveServer = 0;
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)  
  _socket.setsockopt(ZMQ_CURVE_SERVER, &curveServer, sizeof(curveServer));
  _socket.setsockopt(ZMQ_CURVE_SERVERKEY, upstreamPubKey.c_str(), upstreamPubKey.size());
  _socket.setsockopt(ZMQ_CURVE_SECRETKEY, privateKey.c_str(), privateKey.size());
  _socket.setsockopt(ZMQ_CURVE_PUBLICKEY, pubKey.c_str(), pubKey.size());
#else
	L_TRACE("setting curve server");
  _socket.set(zmq::sockopt::curve_server, curveServer);
	L_TRACE("setting curve upstreamPubKey");
  _socket.set(zmq::sockopt::curve_serverkey, upstreamPubKey);
	L_TRACE("setting curve privateKey");
  _socket.set(zmq::sockopt::curve_secretkey, privateKey);
	L_TRACE("setting curve pubKey");
  _socket.set(zmq::sockopt::curve_publickey, pubKey);
#endif

  string url = "tcp://" + upstream + ":" + to_string(port);
	L_TRACE("connecting to ZMQ " << url);
  _socket.connect(url);
	L_INFO("Connect to ZMQ " << name << " at "<< url);
	
  _monitor.reset(new Monitor(_server, name));
  _monitor->init(_socket, "inproc://" + name, ZMQ_EVENT_CONNECTED + ZMQ_EVENT_DISCONNECTED);

}

void Upstream::check() {

//  L_TRACE("checking events");
  _monitor->check_event(20);

}

void Monitor::on_event_connected(const zmq_event_t& event, const char* addr) {

  L_INFO("connected to " << _name);
  _connected = true;
  
  if (_name == "remoteDataReq") {
    _server->sendUpOnline();
  }
  
}   

void Monitor::on_event_disconnected(const zmq_event_t& event, const char* addr) {
  L_WARNING("disconnected from " << _name);
  _connected = false;
}
