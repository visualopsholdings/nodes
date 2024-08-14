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
class InfoRow;
class Upstream;

typedef function<void (json &json)> msgHandler;

class Server {

public:
  Server(bool test, int pub, int rep, int dataReq, int msgSub, const string &dbConn, const string &dbName, const string &certFile, const string &chainFile);
  ~Server();
  
  void run();
  bool resetServer();
  void connectUpstream();
  void goOnline();

private:

  shared_ptr<zmq::context_t> _context;
  shared_ptr<zmq::socket_t> _pub;
  shared_ptr<zmq::socket_t> _rep;
  shared_ptr<Upstream> _dataReq;
  shared_ptr<Upstream> _msgSub;
  map<string, msgHandler> _messages;
  string _certFile;
  string _chainFile;
  int _dataReqPort;
  int _msgSubPort;
  bool _test;
  string _pubKey;
  string _serverId;
  string _upstreamId;
  
  void publish(const json &m) {
    sendTo(_pub, m, "publishing");
  }
  void send(const json &m) {
    sendTo(_rep, m, "sending");
  }
  void sendErr(const string &msg);
  void sendAck();
  void sendTo(shared_ptr<zmq::socket_t> socket, const json &j, const string &type);
  json receiveFrom(shared_ptr<zmq::socket_t> socket);
  bool setInfo(const string &name, const string &text);
  optional<string> getInfo(const vector<InfoRow> &infos, const string &type) const;
  
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
  void siteMsg(json &json);
  void setsiteMsg(json &json);
  
};

#endif // H_server
