/*
  downstream.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Oct-2024
    
  A downstream socket, handles encryption.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_downstream
#define H_downstream

#include <zmq.hpp>

using namespace std;

class Server;

class Downstream {

public:
  Downstream(Server *_server, zmq::context_t &context, int type, const string &name, int port, 
    const string &privateKey);
  
  zmq::socket_t &socket() { return _socket; }
   
private:
  zmq::socket_t _socket;
  
};

#endif // H_upstream
