/*
  nodes.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-May-2024
    
  Command line driver for Nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "log.hpp"

#include <iostream>
#include <boost/program_options.hpp> 
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char *argv[]) {

  string version = "Nodes v0.6.0, 22-Nov-2024.";
  
  int pubPort;
  int repPort;
  string logLevel;
  string dbName;
  string dbConn;
  string hostName;
  string certFile;
  string chainFile;
  bool test;
  bool noupstream;
  int dataReqPort;
  int msgSubPort;
  int remoteDataReqPort;
  int remoteMsgSubPort;
  string bindAddress;
  string schema;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("schema", po::value<string>(&schema)->default_value(""), "The place to get the schema from. Defaults to $HOME/nodes/scripts/schema.json")
    ("pubPort", po::value<int>(&pubPort)->default_value(3012), "ZMQ Pub port.")
    ("repPort", po::value<int>(&repPort)->default_value(3013), "ZMQ Rep port.")
    ("dataReqPort", po::value<int>(&dataReqPort)->default_value(0), "ZMQ Data Req port.")
    ("msgSubPort", po::value<int>(&msgSubPort)->default_value(0), "ZMQ Msg Sub port.")
    ("bindAddress", po::value<string>(&bindAddress)->default_value("127.0.0.1"), "The address to bind to for external access.")    
    ("remoteDataReqPort", po::value<int>(&remoteDataReqPort)->default_value(8810), "ZMQ remote Data Req port.")
    ("remoteMsgSubPort", po::value<int>(&remoteMsgSubPort)->default_value(8811), "ZMQ Remote Msg Sub port.")
    ("logLevel", po::value<string>(&logLevel)->default_value("info"), "Logging level [trace, debug, warn, info].")
    ("dbConn", po::value<string>(&dbConn)->default_value("mongodb://127.0.0.1:27017"), "DB Connection string.")
    ("dbName", po::value<string>(&dbName)->default_value("dev"), "DB name.")
    ("hostName", po::value<string>(&hostName)->default_value("localhost"), "The hostname (for links).")
    ("certFile", po::value<string>(&certFile)->default_value(""), "Certificate file for SSL.")
    ("chainFile", po::value<string>(&chainFile)->default_value(""), "Certificate chain file for SSL.")
    ("test", po::bool_switch(&test), "We are testing so don't use VIDs.")
    ("noupstream", po::bool_switch(&noupstream), "Ignore upstream settings at start. A 'reload' message will try them though")
    ("help", "produce help message")
    ;
  po::positional_options_description p;

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);
  po::notify(vm);   

  Log::config("Nodes    ", logLevel);

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  
  L_INFO(version);

  Server server(test, noupstream, 
    pubPort, repPort, dataReqPort, msgSubPort, remoteDataReqPort, remoteMsgSubPort, 
    dbConn, dbName, schema, certFile, chainFile, hostName, bindAddress);
  
  if (noupstream) {
    L_INFO("ignoring upstream.");
  }
  else {
    server.connectUpstream();
  }
  
  server.run();
  
}
