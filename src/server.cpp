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

Server::Server(int pub, int rep) {

  _context.reset(new zmq::context_t(1));
  _pub.reset(new zmq::socket_t(*_context, ZMQ_PUB));
  _pub->bind("tcp://127.0.0.1:" + to_string(pub));
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local PUB on " << pub;

  _rep.reset(new zmq::socket_t(*_context, ZMQ_REP));
  _rep->bind("tcp://127.0.0.1:" + to_string(rep));
	BOOST_LOG_TRIVIAL(info) << "Connect to ZMQ as Local REP on " << rep;

  _messages["login"] = bind(&Server::loginMsg, this, placeholders::_1, placeholders::_2);
  _messages["streams"] = bind(&Server::streamsMsg, this, placeholders::_1, placeholders::_2);
  _messages["policyusers"] = bind(&Server::policyUsersMsg, this, placeholders::_1, placeholders::_2);
  _messages["message"] = bind(&Server::messageMsg, this, placeholders::_1, placeholders::_2);

  _storage.reset(new Storage());
  
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
        if (!getString(&doc, "type", &type)) {
          BOOST_LOG_TRIVIAL(error) << "no type";
          continue;
        }

        map<string, msgHandler>::iterator handler = _messages.find(type);
        if (handler == _messages.end()) {
          BOOST_LOG_TRIVIAL(error) << "unknown msg type " << type;
          continue;
        }
        handler->second(&doc, _storage);
      }
      catch (zmq::error_t &e) {
        BOOST_LOG_TRIVIAL(warning) << "got exc with rep recv" << e.what() << "(" << e.num() << ")";
      }
    }
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

bool Server::getString(json *j, const string &name, string *value) {

  try {
    *value = boost::json::value_to<string>(j->at(name));
    return true;
  }
  catch (const boost::system::system_error& ex) {
    return false;
  }

}

bool Server::getId(json *j, string *id) {

  try {
    *id = boost::json::value_to<string>(j->at("_id").at("$oid"));
    return true;
  }
  catch (...) {
    return false;
  }
}

void Server::streamsMsg(json *j, shared_ptr<Storage> storage) {

  string user;
  if (!getString(j, "user", &user)) {
    BOOST_LOG_TRIVIAL(error) << "no user";
    return;
  }
  send({
    { "type", "streams" },
    { "user", user },
    { "streams", {
      { { "name", "My Conversation 1" }, { "id", "s1" }, { "policy", "p1" } },
      { { "name", "My Conversation 2" }, { "id", "s2" }, { "policy", "p2" } }
      } 
    }
  });

}

void Server::policyUsersMsg(json *j, shared_ptr<Storage> storage) {

  string policy;
  if (!getString(j, "policy", &policy)) {
    BOOST_LOG_TRIVIAL(error) << "no policy";
    return;
  }
  if (policy != "p1") {
    send({ { "type", "err" }, { "msg", "not correct policy" } });
    return;
  }
  send({
    { "type", "policyusers" },
    { "id", "p1" },
    { "users", {
      { { "id", "u1" }, { "name", "tracy" }, { "fullname", "Tracy" } },
      { { "id", "u2" }, { "name", "leanne" }, { "fullname", "Leanne" } }  
      } 
    }
  });
  
}

void Server::messageMsg(json *j, shared_ptr<Storage> storage) {

  string user;
  if (!getString(j, "user", &user)) {
    BOOST_LOG_TRIVIAL(error) << "no user";
    return;
  }
  string stream;
  if (!getString(j, "stream", &stream)) {
    BOOST_LOG_TRIVIAL(error) << "no stream";
    return;
  }
  if (stream != "s1") {
    send({ { "type", "err" }, { "msg", "not correct stream" } });
    return;
  }
  string policy;
  if (!getString(j, "policy", &policy)) {
    BOOST_LOG_TRIVIAL(error) << "no policy";
    return;
  }
  if (policy != "p1") {
    send({ { "type", "err" }, { "msg", "policy" } });
    return;
  }
  string text;
  if (!getString(j, "text", &text)) {
    BOOST_LOG_TRIVIAL(error) << "no text";
    return;
  }
  if (text == "hello") {
    send({
      { "type", "message" },
      { "text", "world" },
      { "stream", "s1" },
      { "policy", "p1" },
      { "user", user == "u1" ? "u2": "u1" }
    });
    return;
  }
  BOOST_LOG_TRIVIAL(info) << "got " << text << " from " << user;
  send({ { "type", "ack" } });

}