/*
  send.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 16-Apr-2024
    
  Command line driver for Nodes testing.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "log.hpp"
#include "dict.hpp"

#include <iostream>
#include <boost/program_options.hpp> 
#include <zmq.hpp>
#include <boost/algorithm/string.hpp>

namespace po = boost::program_options;

using namespace std;
using namespace vops;

void send(zmq::socket_t &socket, const DictG &obj);
DictG receive(zmq::socket_t &socket);

int main(int argc, char *argv[]) {

  int subPort;
  int reqPort;
  string logLevel;
  string jsonstr;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("subPort", po::value<int>(&subPort)->default_value(3012), "ZMQ Sub port.")
    ("reqPort", po::value<int>(&reqPort)->default_value(3013), "ZMQ Req port.")
    ("logLevel", po::value<string>(&logLevel)->default_value("info"), "Logging level [trace, debug, warn, info].")
    ("json", po::value<string>(&jsonstr)->required(), "The JSON to send")
    ("help", "produce help message")
    ;
  po::positional_options_description p;
  p.add("json", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);
  po::notify(vm);   

  Log::config("Send     ", logLevel);

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  
  zmq::context_t context(1);
  zmq::socket_t req(context, ZMQ_REQ);
  req.connect("tcp://127.0.0.1:" + to_string(reqPort));
	L_TRACE("Connect to ZMQ as Local REQ on " << reqPort);
  
  // input is JSON.
  auto j = Dict::parseString(jsonstr, ".json");
  send(req, *j);
  auto jr = receive(req);
  L_DEBUG("<- " << Dict::toString(jr));
  // receive JSON for now.
  cout << Dict::toString(jr, false, ".json");
    
  return 0;
 
}

void send(zmq::socket_t &socket, const DictG &json) {

  L_DEBUG("-> " << Dict::toString(json));

  // send JSON for now.
  string m = Dict::toString(json, false, ".json");
  zmq::message_t msg(m.length());
  memcpy(msg.data(), m.c_str(), m.length());
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
    socket.send(msg);
#else
    socket.send(msg, zmq::send_flags::none);
#endif

}

DictG receive(zmq::socket_t &socket) {

  zmq::message_t reply;
#if CPPZMQ_VERSION == ZMQ_MAKE_VERSION(4, 3, 1)
  socket.recv(&reply);
#else
  auto res = socket.recv(reply, zmq::recv_flags::none);
#endif
  string r((const char *)reply.data(), reply.size());

  auto j = Dict::parseString(r, ".json");
  L_DEBUG("<- " << Dict::toString(*j));
  
  return *j;

}
