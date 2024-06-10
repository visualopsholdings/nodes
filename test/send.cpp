/*
  send.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Apr-2024
    
  Command line driver for ZMQChat testing.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqirc
*/

#include <iostream>
#include <boost/program_options.hpp> 
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/json.hpp>
#include <zmq.hpp>
#include <boost/algorithm/string.hpp>

namespace po = boost::program_options;

using namespace std;
using json = boost::json::value;

typedef function<void (zmq::socket_t &socket, const vector<string> &args)> msgHandler;
void send(zmq::socket_t &socket, const json &json);
json receive(zmq::socket_t &socket);
void loginMsg(zmq::socket_t &socket, const vector<string> &args);
void streamsMsg(zmq::socket_t &socket, const vector<string> &args);
void policyUsersMsg(zmq::socket_t &socket, const vector<string> &args);
void messageMsg(zmq::socket_t &socket, const vector<string> &args);

int main(int argc, char *argv[]) {

  int subPort;
  int reqPort;
  string logLevel;
  string cmd;
  string args;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("subPort", po::value<int>(&subPort)->default_value(3012), "ZMQ Sub port.")
    ("reqPort", po::value<int>(&reqPort)->default_value(3013), "ZMQ Req port.")
    ("logLevel", po::value<string>(&logLevel)->default_value("info"), "Logging level [trace, debug, warn, info].")
    ("cmd", po::value<string>(&cmd)->required(), "The command to send")
    ("args", po::value<string>(&args)->default_value(""), "The argument for the command")
    ("help", "produce help message")
    ;
  po::positional_options_description p;

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);
  po::notify(vm);   

  if (logLevel == "trace") {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
  }
  else if (logLevel == "debug") {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
  }
  else if (logLevel == "warn") {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
  }
  else {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
  }

  boost::log::formatter logFmt =
         boost::log::expressions::format("%1%\tSend [%2%]\t%3%")
        %  boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") 
        %  boost::log::expressions::attr< boost::log::trivial::severity_level>("Severity")
        %  boost::log::expressions::smessage;
  boost::log::add_common_attributes();
  boost::log::add_console_log(clog)->set_formatter(logFmt);

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  
  map<string, msgHandler> handlers;
  handlers["login"] = bind(&loginMsg, placeholders::_1, placeholders::_2);
  handlers["streams"] = bind(&streamsMsg, placeholders::_1, placeholders::_2);
  handlers["policyusers"] = bind(&policyUsersMsg, placeholders::_1, placeholders::_2);
  handlers["message"] = bind(&messageMsg, placeholders::_1, placeholders::_2);

  zmq::context_t context(1);
  zmq::socket_t req(context, ZMQ_REQ);
  req.connect("tcp://127.0.0.1:" + to_string(reqPort));
  
  vector<string> arglist;
  boost::split(arglist, args, boost::is_any_of(","));
  
  map<string, msgHandler>::iterator handler = handlers.find(cmd);
  if (handler == handlers.end()) {
    BOOST_LOG_TRIVIAL(error) << "unknown cmd " << cmd;
    return 1;
  }
  handler->second(req, arglist);
  
  json jr = receive(req);
  stringstream ss;
  ss << jr;
  cout << ss.str();
  
  return 0;
 
}

void send(zmq::socket_t &socket, const json &json) {

  stringstream ss;
  ss << json;
  string m = ss.str();
  zmq::message_t msg(m.length());
  memcpy(msg.data(), m.c_str(), m.length());
  socket.send(msg);

}

json receive(zmq::socket_t &socket) {

  zmq::message_t reply;
  socket.recv(&reply);
  string r((const char *)reply.data(), reply.size());
  return boost::json::parse(r);

}

void loginMsg(zmq::socket_t &socket, const vector<string> &args) {

  if (args.size() != 1) {
    BOOST_LOG_TRIVIAL(error) << "usage: login password";
    return;
  }
  
  send(socket, {
    { "type", "login" },
    { "session", "1" },
    { "password", args[0] }
  });

}

void streamsMsg(zmq::socket_t &socket, const vector<string> &args) {

  if (args.size() != 1) {
    BOOST_LOG_TRIVIAL(error) << "usage: streams pasusersword";
    return;
  }
  
  send(socket, {
    { "type", "streams" },
    { "user", args[0] }
  });

}

void policyUsersMsg(zmq::socket_t &socket, const vector<string> &args) {

  if (args.size() != 1) {
    BOOST_LOG_TRIVIAL(error) << "usage: policyusers policy";
    return;
  }
  
  send(socket, {
    { "type", "policyusers" },
    { "policy", args[0] }
  });

}

void messageMsg(zmq::socket_t &socket, const vector<string> &args) {

   if (args.size() != 4) {
    BOOST_LOG_TRIVIAL(error) << "usage: message user stream policy text";
    return;
  }
  
  send(socket, {
    { "type", "message" },
    { "user", args[0] },
    { "stream", args[1] },
    { "policy", args[2] },
    { "text", args[3] }
  });

}