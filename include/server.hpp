/*
  server.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
    
  The main server for Nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_server
#define H_server

#include <zmq.hpp>
#include <boost/json.hpp>
#include <map>

using namespace std;
using json = boost::json::value;

class DynamicRow;

typedef function<void (json &json)> msgHandler;

class Server {

public:
  Server(bool test, int pub, int rep, const string &dbConn, const string &dbName, const string &certFile, const string &chainFile);
  ~Server();
  
  void run();
  bool resetServer();
  bool rebootServer();

private:

  shared_ptr<zmq::context_t> _context;
  shared_ptr<zmq::socket_t> _pub;
  shared_ptr<zmq::socket_t> _rep;
  map<string, msgHandler> _messages;
  string _certFile;
  string _chainFile;
  bool _test;
  
  void publish(const json &m) {
    sendTo(_pub, m, "publishing");
  }
  void send(const json &m) {
    sendTo(_rep, m, "sending");
  }
  void sendErr(const string &msg);
  void sendAck();
  void sendTo(shared_ptr<zmq::socket_t> _socket, const json &j, const string &type);
  bool setInfo(const string &name, const string &text);
  
  // handlers
  void loginMsg(json &json);
  void policyUsersMsg(json &json);
  void messageMsg(json &json);
  void certsMsg(json &json);
  void usersMsg(json &json);
  void userMsg(json &json);
  void streamsMsg(json &json);
  void streamMsg(json &json);
  void ideasMsg(json &json);
  void infosMsg(json &json);
  void setinfoMsg(json &json);
  
};

#endif // H_server
