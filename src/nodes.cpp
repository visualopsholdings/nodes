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

#include <iostream>
#include <boost/program_options.hpp> 
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char *argv[]) {

  int pubPort;
  int repPort;
  string logLevel;
  string dbName;
  string dbConn;
  string certFile;
  string chainFile;
  bool test;
  bool noupstream;
  int dataReqPort;
  int msgSubPort;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("pubPort", po::value<int>(&pubPort)->default_value(3012), "ZMQ Pub port.")
    ("repPort", po::value<int>(&repPort)->default_value(3013), "ZMQ Rep port.")
    ("dataReqPort", po::value<int>(&dataReqPort)->default_value(8810), "ZMQ Data Req port.")
    ("msgSubPort", po::value<int>(&msgSubPort)->default_value(8811), "ZMQ Msg Sub port.")
    ("logLevel", po::value<string>(&logLevel)->default_value("info"), "Logging level [trace, debug, warn, info].")
    ("dbConn", po::value<string>(&dbConn)->default_value("mongodb://127.0.0.1:27017"), "DB Connection string.")
    ("dbName", po::value<string>(&dbName)->default_value("dev"), "DB name.")
    ("certFile", po::value<string>(&certFile)->default_value(""), "Certificate file for SSL.")
    ("chainFile", po::value<string>(&chainFile)->default_value(""), "Certificate chain file for SSL.")
    ("test", po::bool_switch(&test), "We are testing so don't use VIDs.")
    ("noupstream", po::bool_switch(&noupstream), "Ignore upstream settings.")
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
         boost::log::expressions::format("%1%\tNodes     [%2%]\t%3%")
        %  boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") 
        %  boost::log::expressions::attr< boost::log::trivial::severity_level>("Severity")
        %  boost::log::expressions::smessage;
  boost::log::add_common_attributes();
  boost::log::add_console_log(clog)->set_formatter(logFmt);

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  
  BOOST_LOG_TRIVIAL(info) << "Nodes 0.4, 29-Jul-2024.";

  Server server(test, noupstream, pubPort, repPort, dataReqPort, msgSubPort, dbConn, dbName, certFile, chainFile);
  server.run();
  
}
