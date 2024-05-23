/*
  server.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
    
  The main server for ZMQChat
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_server
#define H_server

#include <zmq.hpp>
#include <boost/json.hpp>
#include <map>

using namespace std;
using json = boost::json::value;

typedef function<void (json *json)> msgHandler;

class Server {

public:
  Server(int pub, int rep);
  ~Server();
  
  void run();

private:

  shared_ptr<zmq::context_t> _context;
  shared_ptr<zmq::socket_t> _pub;
  shared_ptr<zmq::socket_t> _rep;
  map<string, msgHandler> _messages;
  
  void send(const json &m);
  bool getString(json *j, const string &name, string *value);

  // handlers
  void loginMsg(json *json);
  void streamsMsg(json *json);
  void policyUsersMsg(json *json);
  void messageMsg(json *json);
  
};

#endif // H_server
