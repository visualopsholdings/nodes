/*
  downstream.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Oct-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "downstream.hpp"

#include "server.hpp"
#include "log.hpp"

Downstream::Downstream(Server *server, zmq::context_t &context, int type, const string &name, int port, 
    const string &privateKey) : 
      _socket(context, type) {

	L_TRACE("setting curve options");
  int curveServer = 1;
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)  
  _socket.setsockopt(ZMQ_CURVE_SERVER, &curveServer, sizeof(curveServer));
  _socket.setsockopt(ZMQ_CURVE_SECRETKEY, privateKey.c_str(), privateKey.size());
#else
	L_TRACE("setting curve server");
  _socket.set(zmq::sockopt::curve_server, curveServer);
	L_TRACE("setting curve privateKey");
  _socket.set(zmq::sockopt::curve_secretkey, privateKey);
#endif

  // probably need to bind to external interface
  string url = "tcp://" + server->_bindAddress + ":" + to_string(port);
	L_TRACE("Binding to ZMQ " << url);
  _socket.bind(url);
	L_INFO("Bind to ZMQ " << name << " at "<< url);
	
}
