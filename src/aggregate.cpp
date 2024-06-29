/*
  aggregate.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 25-Jun-2024
    
  Command line driver for indexing the collections using an aggregation pipeline.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "storage.hpp"
#include "storage/schema.hpp"

#include <iostream>
#include <boost/program_options.hpp> 
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <fstream>

#include <bsoncxx/json.hpp>
#include <mongocxx/collection.hpp>

using namespace std;
namespace po = boost::program_options;

int main(int argc, char *argv[]) {

  string logLevel;
  string dbConn;
  string dbName;
  string filename;
  string coll;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("logLevel", po::value<string>(&logLevel)->default_value("info"), "Logging level [trace, debug, warn, info].")
    ("filename", po::value<string>(&filename)->required(), "Aggregation JSON.")
    ("collection, coll", po::value<string>(&coll)->required(), "The collection [group | policy].")
    ("dbConn", po::value<string>(&dbConn)->default_value("mongodb://127.0.0.1:27017"), "DB Connection string.")
    ("dbName", po::value<string>(&dbName)->default_value("dev"), "DB name.")
    ("help", "produce help message")
    ;
  po::positional_options_description p;
  p.add("filename", -1);

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
         boost::log::expressions::format("%1%\tAggregate [%2%]\t%3%")
        %  boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") 
        %  boost::log::expressions::attr< boost::log::trivial::severity_level>("Severity")
        %  boost::log::expressions::smessage;
  boost::log::add_common_attributes();
  boost::log::add_console_log(clog)->set_formatter(logFmt);

  BOOST_LOG_TRIVIAL(info) << "aggregating collection " << coll;
  
  Storage::instance()->init(dbConn, dbName);
  
  if (coll == "group") {
    Group().aggregate(filename);
  }
  else if (coll == "policy") {
    Policy().aggregate(filename);
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "unknown collection " << coll;
  }
}
