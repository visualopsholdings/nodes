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
  Server(bool test, bool noupstream, int pub, int rep, int dataReq, int msgSub, const string &dbConn, 
    const string &dbName, const string &certFile, const string &chainFile, const string &hostName);
  ~Server();
  
  void run();
  bool resetServer();
  void connectUpstream();
  void clearUpstream();
  void stopUpstream();
  void online();
  void heartbeat();
  void systemStatus(const string &msg);
  void discover();

  void publish(const json &m) {
    sendTo(*_pub, m, "publishing");
  }
  void send(const json &m) {
    sendTo(*_rep, m, "sending");
  }
  void sendErr(const string &msg);
  void sendWarning(const string &msg);
  void sendAck();
  void sendDataReq(const json &m);
  bool setInfo(const string &name, const string &text);
  optional<string> getInfo(const vector<InfoRow> &infos, const string &type) const;
  string get1Info(const string &type);

  string _certFile;
  string _chainFile;
  string _hostName;
  bool _test;
  bool _online;
  string _serverId;
  string _upstreamId;
  bool _reload;
  
private:

  shared_ptr<zmq::context_t> _context;
  shared_ptr<zmq::socket_t> _pub;
  shared_ptr<zmq::socket_t> _rep;
  shared_ptr<Upstream> _dataReq;
  shared_ptr<Upstream> _msgSub;
  map<string, msgHandler> _messages;
  map<string, msgHandler> _dataReqMessages;
  int _dataReqPort;
  int _msgSubPort;
  string _pubKey;
  time_t _lastHeartbeat;
  bool _noupstream;
  
  void sendTo(zmq::socket_t &socket, const json &j, const string &type);
  json receiveFrom(shared_ptr<zmq::socket_t> socket);
  bool getMsg(const string &name, zmq::socket_t &socket, map<string, msgHandler> &handlers );
    
};

#endif // H_server
