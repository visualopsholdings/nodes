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
#include "data.hpp"

#include <zmq.hpp>
#include <boost/json.hpp>
#include <map>
#ifdef MONGO_DB
#include <mongocxx/collection.hpp>
#endif

using namespace std;
using json = boost::json::value;
using namespace vops;

namespace nodes {

class DynamicRow;
class InfoRow;
class Upstream;
class Downstream;
class Data;

typedef function<void (Data &data)> msgHandler;

typedef struct {
  optional<long> time; // this could be 0
  optional<string> modifyDate;
} IncomingMsgTest;
typedef struct {
  string type;
  optional<string> objtype;
  optional<string> me;
  optional<IncomingMsgTest> test;
  optional<vector<string> > path;
  rfl::ExtraFields<DictG> extra_fields;
} IncomingMsg;

typedef function<void (const IncomingMsg &in)> msgHandler2;

class Server {

public:
  Server(bool test, bool noupstream, 
    int pub, int rep, int dataRep, int msgPub, int remoteDataReq, int remoteMsgSub,
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
  
  void publish(optional<string> corr, const json &m) {
    sendTo(*_pub, m, "*-> ", corr);
  }
  void send(const json &m) {
    sendTo(*_rep, m, "-> ", nullopt);
  }
  void send(const DictO &m) {
    sendTo(*_rep, m, "-> ", nullopt);
  }
  void send(const std::string &s) {
    sendTo(*_rep, s, "-> ");
  }
  void setSrc(boost::json::object *m);
  void setSrc(DictO *m);
  bool getSrc(const IncomingMsg &in, string *s);
  bool getSrc(json &msg, string *s);
  void sendDown(const DictO &m);
  void sendDown(const std::string &s);
  void pubDown(const DictO &m);
  void pubDown(const json &m);
  void sendOn(const DictO &m);
  void sendOn(const Data &m) {
    sendOn(m.dict());
  }
  void sendErr(const string &msg);
  void sendErrDown(const string &msg);
  void sendWarning(const string &msg);
  void sendSecurity();
  void sendAck(optional<string> result=nullopt);
  void sendAckDown(optional<string> result=nullopt);
  void sendDataReq(optional<string> corr, const DictO &m);
  void sendDataReq(optional<string> corr, const json &m);
  
  // notifying other nodes.
  void sendUpd(const string &type, const string &id, const DictO &data);
  void sendUpd(const string &type, const string &id, Data &data) {
    sendUpd(type, id, data.dict());
  }
  void sendAdd(const string &type, const DictO &data);
  void sendAdd(const string &type, Data &data) {
    sendAdd(type, data.dict());
  }
  void sendMov(const string &type, const string &id, const DictO &obj, const string &ptype, const string &origparent);
  void sendMov(const string &type, const string &id, Data &obj, const string &ptype, const string &origparent) {
    sendMov(type, id, obj.dict(), ptype, origparent);
  }
  
  bool setInfo(const string &name, const string &text);
  string get1Info(const string &type);
    // dealing with the "infos"
    
  void sendCollection(const IncomingMsg &m, const string &name, const DictV &array);
    //. send a collection back, taking care of the test for latest
    
  void sendObject(const IncomingMsg &m, const string &name, const DictG &obj);
    // send an object back, taking care of the test for latest
    
  bool testModifyDate(json &j, const json &doc);
  bool testModifyDate(const IncomingMsg &m, const DictG &obj);
    // test for the modifyDate to be the latest.
    
  void importObjs(const DictV &msgs);
    // given msgs in the format of { "type": "user", "objs": [obj, obj] }
    // import them.
    
  bool updateObject(const DictO &j);
  bool updateObject(Data &j) {
    return updateObject(j.dict());
  }
    // given an object in the format {"data":{"type":"user","id":"6121bdfaec9e5a059715739c","obj":obj }}
    // update it
    
  bool addObject(const DictO &j);
  bool addObject(Data &j) {
    return addObject(j.dict());
  }
    // given an object in the format {"data":{"type":"user","obj":obj }}
    // update it
    
  bool shouldIgnoreAdd(const DictO &msg);
  bool shouldIgnoreAdd(const Data &msg) {
    return shouldIgnoreAdd(msg.dict());
  }
    // adds always come, should we ignore one?

  bool wasFromUs(const DictO &msg);
  bool wasFromUs(const Data &msg);
    // the message was send from us.

  string _hostName;
  bool _test;
  bool _online;
  string _serverId;
  string _upstreamId;
  bool _reload;
  string _bindAddress;
  
private:

  shared_ptr<zmq::context_t> _context;
  shared_ptr<zmq::socket_t> _pub;
  shared_ptr<zmq::socket_t> _rep;
  shared_ptr<Upstream> _remoteDataReq;
  shared_ptr<Upstream> _remoteMsgSub;
  shared_ptr<Downstream> _dataRep;
  shared_ptr<Downstream> _msgPub;
  map<string, msgHandler> _messages;
  map<string, msgHandler2> _messages2;
  map<string, msgHandler> _remoteDataReqMessages;
  map<string, msgHandler2> _remoteDataReqMessages2;
  map<string, msgHandler> _dataRepMessages;
  map<string, msgHandler2> _dataRepMessages2;
  map<string, msgHandler> _remoteMsgSubMessages;
  int _remoteDataReqPort;
  int _remoteMsgSubPort;
  int _dataRepPort;
  int _msgPubPort;
  string _pubKey;
  time_t _lastHeartbeat;
  bool _noupstream;
  string _certFile;
  string _chainFile;
  
  void sendTo(zmq::socket_t &socket, const json &j, const string &type, optional<string> corr);
  void sendTo(zmq::socket_t &socket, const DictO &j, const string &type, optional<string> corr);
  void sendTo(zmq::socket_t &socket, const std::string &j, const string &type);
  json receiveFrom(shared_ptr<zmq::socket_t> socket);
  bool getMsg(const string &name, zmq::socket_t &socket, map<string, msgHandler> &handlers, std::optional<map<string, msgHandler2> > handlers2);
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
  std::string buildCollResultJson(const IncomingMsg &in, const std::string &name, const DictV &array);
  std::string buildObjResultJson(const IncomingMsg &in, const std::string &name, const DictG &obj);
 
};

} // nodes

#endif // H_server
