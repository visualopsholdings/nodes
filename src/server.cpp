/*
  server.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"

#include <boost/log/trivial.hpp>

Server::Server(bool test, int pub, int rep, const string &dbConn, const string &dbName, const string &certFile, const string &chainFile) :
    _test(test), _certFile(certFile), _chainFile(chainFile) {

  _context.reset(new zmq::context_t(1));
  _pub.reset(new zmq::socket_t(*_context, ZMQ_PUB));
  _pub->bind("tcp://127.0.0.1:" + to_string(pub));
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local PUB on " << pub;

  _rep.reset(new zmq::socket_t(*_context, ZMQ_REP));
  _rep->bind("tcp://127.0.0.1:" + to_string(rep));
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local REP on " << rep;

  _messages["certs"] = bind(&Server::certsMsg, this, placeholders::_1);
  _messages["login"] = bind(&Server::loginMsg, this, placeholders::_1);
  _messages["streams"] = bind(&Server::streamsMsg, this, placeholders::_1);
  _messages["policyusers"] = bind(&Server::policyUsersMsg, this, placeholders::_1);
  _messages["message"] = bind(&Server::messageMsg, this, placeholders::_1);

  Storage::instance()->init(dbConn, dbName);
  
}
  
Server::~Server() {
}
  
void Server::run() {

  zmq::pollitem_t items [] = {
      { *_rep, 0, ZMQ_POLLIN, 0 }
  };
  const std::chrono::milliseconds timeout{500};
  while (1) {
  
//      BOOST_LOG_TRIVIAL(debug) << "polling for messages";
    zmq::message_t message;
    zmq::poll(&items[0], 1, timeout);
  
    if (items[0].revents & ZMQ_POLLIN) {
      BOOST_LOG_TRIVIAL(debug) << "got rep message";
      zmq::message_t reply;
      try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
        _rep->recv(&reply);
#else
        auto res = _rep->recv(reply, zmq::recv_flags::none);
#endif
        // convert to JSON
        string r((const char *)reply.data(), reply.size());
        json doc = boost::json::parse(r);

        BOOST_LOG_TRIVIAL(debug) << "got reply " << doc;

        string type;
        if (!getString(doc, "type", &type)) {
          BOOST_LOG_TRIVIAL(error) << "no type";
          continue;
        }

        map<string, msgHandler>::iterator handler = _messages.find(type);
        if (handler == _messages.end()) {
          BOOST_LOG_TRIVIAL(error) << "unknown msg type " << type;
          continue;
        }
        handler->second(doc);
      }
      catch (zmq::error_t &e) {
        BOOST_LOG_TRIVIAL(warning) << "got exc with rep recv" << e.what() << "(" << e.num() << ")";
      }
    }
  }

}

void Server::publish(const json &j) {

  stringstream ss;
  ss << j;
  string m = ss.str();
  
  BOOST_LOG_TRIVIAL(info) << "publishing " << m;

	zmq::message_t msg(m.length());
	memcpy(msg.data(), m.c_str(), m.length());
  try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
    _pub->send(msg);
#else
    _pub->send(msg, zmq::send_flags::none);
#endif
  }
  catch (zmq::error_t &e) {
    BOOST_LOG_TRIVIAL(warning) << "got exc publish" << e.what() << "(" << e.num() << ")";
  }

}

void Server::send(const json &j) {

  stringstream ss;
  ss << j;
  string m = ss.str();
  
  BOOST_LOG_TRIVIAL(info) << "sending " << m;

	zmq::message_t msg(m.length());
	memcpy(msg.data(), m.c_str(), m.length());
  try {
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
    _rep->send(msg);
#else
    _rep->send(msg, zmq::send_flags::none);
#endif
  }
  catch (zmq::error_t &e) {
    BOOST_LOG_TRIVIAL(warning) << "got exc send" << e.what() << "(" << e.num() << ")";
  }

}

void Server::sendErr(const string &msg) {
  BOOST_LOG_TRIVIAL(error) << msg;
  send({ { "type", "err" }, { "msg", msg } });
}

void Server::sendAck() {
  send({ { "type", "ack" } });
}

bool Server::getString(optional<json> &j, const string &name, string *value) {

  if (!j) {
    return false;
  }
  
  return getString(j.value(), name, value);

}

bool Server::getString(json &j, const string &name, string *value) {

  try {
    *value = boost::json::value_to<string>(j.at(name));
    return true;
  }
  catch (const boost::system::system_error& ex) {
    return false;
  }

}

bool Server::getArray(optional<json> &j, const string &name, vector<string> *value) {

  if (!j) {
    return false;
  }
  
  return getArray(j.value(), name, value);

}

bool Server::getArray(json &j, const string &name, vector<string> *value) {

  if (!j.as_object().if_contains(name)) {
    return false;
  }
  if (!j.at(name).is_array()) {
    return false;
  }
  value->clear();
  for (auto i: j.at(name).as_array()) {
    try {
      value->push_back(boost::json::value_to<string>(i));
    }
    catch (...) {
      return false;
    }
  }
  return true;
}
