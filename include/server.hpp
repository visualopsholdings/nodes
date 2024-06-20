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

class Storage;

typedef function<void (json &json, shared_ptr<Storage> storage)> msgHandler;

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
  shared_ptr<Storage> _storage;
  
  void publish(const json &j);
  void send(const json &m);
  void sendErr(const string &msg);
  void sendAck();
  bool getString(json &j, const string &name, string *value);
  bool getString(optional<json> &j, const string &name, string *value);
  bool getArray(json &j, const string &name, vector<string> *value);
  bool getArray(optional<json> &j, const string &name, vector<string> *value);

  // handlers
  void loginMsg(json &json, shared_ptr<Storage> storage);
  void streamsMsg(json &json, shared_ptr<Storage> storage);
  void policyUsersMsg(json &json, shared_ptr<Storage> storage);
  void messageMsg(json &json, shared_ptr<Storage> storage);
  void certsMsg(json &json, shared_ptr<Storage> storage);
  
};

#endif // H_server
