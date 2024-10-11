/*
  counts.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 10-Oct-2024
    
  Command line driver for determining object counts for testing
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage.hpp"
#include "json.hpp"

#include <iostream>
#include <boost/program_options.hpp> 
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/json.hpp>
#include <boost/algorithm/string.hpp>

namespace po = boost::program_options;

using namespace std;
using json = boost::json::value;

int main(int argc, char *argv[]) {

  string logLevel;
  string dbName;
  string dbConn;
  int users;
  int groups;
  int streams;
  int ideas;
  int policies;
  int fullUsers;
  int waitingUsers;
  int fullGroups;
  int waitingGroups;
  int fullStreams;
  int waitingStreams;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("users", po::value<int>(&users)->default_value(-1), "Users")
    ("groups", po::value<int>(&groups)->default_value(-1), "Groups")
    ("streams", po::value<int>(&streams)->default_value(-1), "Streams")
    ("ideas", po::value<int>(&ideas)->default_value(-1), "Ideas")
    ("policies", po::value<int>(&policies)->default_value(-1), "Policies")
    ("fullUsers", po::value<int>(&fullUsers)->default_value(-1), "Full Users")
    ("waitingUsers", po::value<int>(&waitingUsers)->default_value(-1), "Waiting Users")
    ("fullGroups", po::value<int>(&fullGroups)->default_value(-1), "Full Groups")
    ("waitingGroups", po::value<int>(&waitingGroups)->default_value(-1), "Waiting Groups")
    ("fullStreams", po::value<int>(&fullStreams)->default_value(-1), "Full Streams")
    ("waitingStreams", po::value<int>(&waitingStreams)->default_value(-1), "Waitkling Streams")
    ("dbConn", po::value<string>(&dbConn)->default_value("mongodb://127.0.0.1:27017"), "DB Connection string.")
    ("dbName", po::value<string>(&dbName)->default_value("dev"), "DB name.")
    ("logLevel", po::value<string>(&logLevel)->default_value("info"), "Logging level [trace, debug, warn, info].")
    ("help", "produce help message")
    ;
  po::positional_options_description p;
  p.add("json", -1);

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
         boost::log::expressions::format("%1%\tSend      [%2%]\t%3%")
        %  boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") 
        %  boost::log::expressions::attr< boost::log::trivial::severity_level>("Severity")
        %  boost::log::expressions::smessage;
  boost::log::add_common_attributes();
  boost::log::add_console_log(clog)->set_formatter(logFmt);

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  
  Storage::instance()->init(dbConn, dbName);

  boost::json::array lines;
  if (users >= 0) {
    lines.push_back({ 
      { "type", "users" }, 
      { "expected", users }
    });
  }
  if (fullUsers >= 0) {
    lines.push_back({ 
      { "type", "users" }, 
      { "expected", fullUsers },
      { "full", true },
      { "field", "fullname" }
    });
  }
  if (waitingUsers >= 0) {
    lines.push_back({ 
      { "type", "users" }, 
      { "expected", waitingUsers },
      { "waiting", true },
      { "field", "fullname" }
    });
  }

  if (groups >= 0) {
    lines.push_back({ 
      { "type", "groups" }, 
      { "expected", groups }
    });
  }
  if (fullGroups >= 0) {
    lines.push_back({ 
      { "type", "groups" }, 
      { "expected", fullGroups },
      { "full", true },
      { "field", "name" }
    });
  }
  if (waitingGroups >= 0) {
    lines.push_back({ 
      { "type", "groups" }, 
      { "expected", waitingGroups },
      { "waiting", true },
      { "field", "name" }
    });
  }

  if (streams >= 0) {
    lines.push_back({ 
      { "type", "streams" }, 
      { "expected", streams }
    });
  }
  if (fullStreams >= 0) {
    lines.push_back({ 
      { "type", "streams" }, 
      { "expected", fullStreams },
      { "full", true },
      { "field", "name" }
    });
  }
  if (waitingStreams >= 0) {
    lines.push_back({ 
      { "type", "streams" }, 
      { "expected", waitingStreams },
      { "waiting", true },
      { "field", "name" }
    });
  }

  if (ideas >= 0) {
    lines.push_back({ 
      { "type", "ideas" }, 
      { "expected", ideas }
    });
  }

  if (policies >= 0) {
    lines.push_back({ 
      { "type", "policies" }, 
      { "expected", policies }
    });
  }

  bool success = true;
  for (auto o: lines) {
    boost::json::object q;
    auto type = Json::getString(o, "type");
    if (!type) {
      continue;
    }
    auto expected = Json::getNumber(o, "expected");
    if (!expected) {
      continue;
    }
    auto field = Json::getString(o, "field", true);
    if (Json::getBool(o, "full", true)) {
      q[field.value()] = {
        { "$ne", "Waiting discovery" }
      };
    }
    else if (Json::getBool(o, "waiting", true)) {
      q[field.value()] = "Waiting discovery";
    }
    
    int count = SchemaImpl::countGeneral(type.value(), q);
    if (count != expected) {
      if (success) {
        success = false;
      }
      cerr << "error: " << type.value() << " got " << count << " expected " << expected.value() << endl;
    }
  }
  
  return !success;
  
//  return 0;
  
}
