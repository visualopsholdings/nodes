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

#include "dict.hpp"

#include <zmq.hpp>
#include <map>
#ifdef MONGO_DB
#include <mongocxx/collection.hpp>
#endif

using namespace std;
using namespace vops;

namespace nodes {

class DynamicRow;
class InfoRow;
class Upstream;
class Downstream;

typedef struct {
  optional<long> time; // this could be 0
  optional<string> modifyDate;
} IncomingMsgTest;
typedef struct {
  string type;
  optional<string> objtype;
  optional<string> me;
  optional<string> id;
  optional<string> corr;
  optional<IncomingMsgTest> test;
  optional<vector<string> > path;
  rfl::ExtraFields<DictG> extra_fields;
} IncomingMsg;

typedef function<void (const IncomingMsg &in)> msgHandler;
typedef function<void (const char *data, size_t size)> binMsgHandler;

class Server {

public:
  Server(bool test, bool noupstream, 
    const string &mediaDir, long maxFileSize, long chunkSize,
    int pub, int rep, 
    int dataRep, int msgPub, int binRep, 
    int remoteDataReq, int remoteMsgSub, int remoteBinReq, 
    const string &dbConn, const string &dbName, const string &schema,
    const string &certFile, const string &chainFile, const string &hostName, const string &bindAddress);
  ~Server();
  
  void run();
  bool resetServer();
  void connectUpstream();
  void clearUpstream();
  void stopUpstream();
  void sendUpOnline();
  void serveOnline();
  void sendUpHeartbeat();
  void systemStatus(const string &msg);
  void sendUpDiscover();
  void sendUpDiscoverLocal(optional<string> corr);
  void sendDownDiscoverResult(const IncomingMsg &in);
  void resetDB();
  void discoveryComplete();
  void discoverBinary();
  
  void publish(optional<string> corr, const DictO &m) {
    sendTo(*_pub, m, "*-> ", corr);
  }
  void send(const DictO &m) {
    sendTo(*_rep, m, "-> ", nullopt);
  }
  void send(const std::string &s) {
    sendTo(*_rep, s, "-> ");
  }
  void setSrc(DictO *m);
  bool getSrc(const IncomingMsg &in, string *s);
  void sendDown(const DictO &m);
  void sendDown(const std::string &s);
  void pubDown(const DictO &m);
  void sendOn(const DictO &m);
  void sendErr(const string &msg);
  void sendErrDown(const string &msg);
  void sendWarning(const string &msg);
  void sendSecurity();
  void sendAck(optional<string> result=nullopt);
  void sendAck(const DictO &result);
  void sendAckDown(optional<string> result=nullopt);
  void sendAckDownBin();
  void sendDataReq(optional<string> corr, const DictO &m);
  void sendBinReq(const DictO &m);
  void sendDownBin(const DictO &m);
  void sendDownBin(const std::string &s);
  void sendDownBin(const std::vector<char> &d);
  
  // notifying other nodes.
  void sendUpd(const string &type, const string &id, const DictO &data);
  void sendAdd(const string &type, const DictO &data);
  void sendMov(const string &type, const string &id, const DictO &obj, const string &ptype, const string &origparent);
  
  bool setInfo(const string &name, const string &text);
  string get1Info(const string &type);
    // dealing with the "infos"
    
  void sendCollection(const IncomingMsg &m, const string &name, const DictV &array);
    //. send a collection back, taking care of the test for latest
    
  void sendObject(const IncomingMsg &m, const string &name, const DictG &obj);
    // send an object back, taking care of the test for latest
    
  bool testModifyDate(const IncomingMsg &m, const DictG &obj);
    // test for the modifyDate to be the latest.
    
  void importObjs(const DictV &msgs);
    // given msgs in the format of { "type": "user", "objs": [obj, obj] }
    // import them.
    
  bool updateObject(const DictO &j);
    // given an object in the format {"data":{"type":"user","id":"6121bdfaec9e5a059715739c","obj":obj }}
    // update it
    
  bool addObject(const DictO &j);
    // given an object in the format {"data":{"type":"user","obj":obj }}
    // update it
    
  bool shouldIgnoreAdd(const DictO &msg);
    // adds always come, should we ignore one?

  bool wasFromUs(const IncomingMsg &in);
  bool wasFromUs(const DictO &msg);
    // the message was send from us.

  optional<DictO> toObject(const IncomingMsg &in);
    // convert an incoming message to an object.
    
  DictO fixObjectForReturn(const DictO &j);
    // after insertion, some objects need to be fixed when returned.
    
  optional<tuple<string, DictG> > firstSchemaBinary(const DictO &obj);
    // collect all the next binary object we need to transfer
    
  string _hostName;
  bool _test;
  bool _online;
  string _serverId;
  string _upstreamId;
  bool _reload;
  string _bindAddress;
  string _mediaDir;
  long _maxFileSize;
  long _chunkSize;
  
private:

  shared_ptr<zmq::context_t> _context;
  shared_ptr<zmq::socket_t> _pub;
  shared_ptr<zmq::socket_t> _rep;
  shared_ptr<Downstream> _dataRep;
  shared_ptr<Downstream> _msgPub;
  shared_ptr<Downstream> _binRep;
  shared_ptr<Upstream> _remoteDataReq;
  shared_ptr<Upstream> _remoteMsgSub;
  shared_ptr<Upstream> _remoteBinReq;
  map<string, msgHandler> _messages;
  map<string, msgHandler> _remoteDataReqMessages;
  vector<binMsgHandler> _remoteBinReqMessages;
  map<string, msgHandler> _dataRepMessages;
  map<string, msgHandler> _binRepMessages;
  map<string, msgHandler> _remoteMsgSubMessages;
  int _remoteDataReqPort;
  int _remoteMsgSubPort;
  int _remoteBinReqPort;
  int _dataRepPort;
  int _msgPubPort;
  int _binRepPort;
  string _pubKey;
  time_t _lastHeartbeat;
  bool _noupstream;
  string _certFile;
  string _chainFile;
  
  void sendTo(zmq::socket_t &socket, const DictO &j, const string &type, optional<string> corr);
  void sendTo(zmq::socket_t &socket, const std::string &j, const string &type);
  bool getMsg(const string &name, zmq::socket_t &socket, map<string, msgHandler> &handlers);
  bool getBinMsg(const string &name, zmq::socket_t &socket, vector<binMsgHandler> &handlers);
  bool testCollectionChanged(const IncomingMsg &m, const string &name);
  void runUpstreamOnly();
  void runStandalone();
  void runUpstreamDownstream();
  void runDownstreamOnly();
  string getLastDate(optional<DictV> rows, const string &hasInitialSync, const string &date, bool all);
  void sendUpDiscoverLocalUpstream(const string &upstreamLastSeen, optional<string> corr);
  void sendUpDiscoverLocalMirror(const string &upstreamLastSeen, optional<string> corr);
#ifdef MONGO_DB
  bool collectObjs(const string &type, const string &collname, bsoncxx::document::view_or_value q, 
    DictV *data, vector<string> *policies, optional<int> limit, bool mark);
#endif
  void collectPolicies(const vector<string> &policies, DictV *data);
  bool isValidId(const string &id);
  bool validateId(const DictO &obj, const string &id);
  bool hasValidNodes();
  bool anyNodesListening(const string &type, const string &id);
  bool shouldSendDown(const string &action, const string &type, const string &id, const DictO &obj);
  vector<string> getNodeIds(const string &type);
  string getCollName(const string &type, optional<string> coll);
  bool isObjParentUpstream(const string &type, const DictO &obj);
  bool isParentUpstream(const string &ptype, const string &origparent);
  bool isObjUpstream(const DictO &obj);
  bool hasUpstream();
  optional<DictV> getSubobjsLatest(const DictO &subobj, const vector<string> &ids, 
    const string &hasInitialSync, const string &upstreamLastSeen, bool collzd);
  void collectIds(const DictV &ids, vector<string> *vids);
  void unmarkAll(const DictO &obj);
  string zeroDate();
  std::string buildErrJson(const std::string &level, const std::string &msg);
  std::string buildAckJson(std::optional<std::string> result);
  std::string buildAckJson(const DictO &result);
  std::string buildCollResultJson(const IncomingMsg &in, const std::string &name, const DictV &array);
  std::string buildObjResultJson(const IncomingMsg &in, const std::string &name, const DictG &obj);
 
};

class SendBinData {

public:
  SendBinData(Server *server): _server(server) {}
  ~SendBinData() {
    _server->sendDownBin(_data);
  }
  
  vector<char> *data() { return &_data; }
  
private:
  Server *_server;
  vector<char> _data;
  
};

} // nodes

#endif // H_server
