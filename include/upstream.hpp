/*
  upstream.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 13-Aug-2024
    
  An upstream socket, handles encryption and monitoring connection
  status.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_upstream
#define H_upstream

#include <zmq.hpp>

using namespace std;

namespace nodes {

class Server;

class Monitor : public zmq::monitor_t {

public:
  Monitor(Server *server, const string &name) : _server(server), _name(name) {}
  
protected:
  void on_event_connected(const zmq_event_t& event, const char* addr) override;
  void on_event_disconnected(const zmq_event_t& event, const char* addr) override;

private:
  friend class Upstream;
  
  Server *_server;
  string _name;
  bool _connected;
};

class Upstream {

public:
  Upstream(Server *_server, zmq::context_t &context, int type, const string &name, const string &upstream, int port, const string &upstreamPubKey, 
    const string &privateKey, const string &pubKey);
  
  void check();
  bool connected() { return _monitor->_connected; }
  zmq::socket_t &socket() { return _socket; }
   
private:
  zmq::socket_t _socket;
  shared_ptr<Monitor> _monitor;
  
};

} // nodes

#endif // H_upstream
