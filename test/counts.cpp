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
#include "log.hpp"

#include <iostream>
#include <boost/program_options.hpp> 
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/json.hpp>
#include <boost/algorithm/string.hpp>

namespace po = boost::program_options;

using namespace std;
using json = boost::json::value;
using namespace nodes;

int main(int argc, char *argv[]) {

  string logLevel;
  string dbName;
  string dbConn;
  int users;
  int groups;
  int collections;
  int objs;
  int policies;
  int fullUsers;
  int waitingUsers;
  int fullGroups;
  int waitingGroups;
  int fullCollections;
  int waitingCollections;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("users", po::value<int>(&users)->default_value(-1), "Users")
    ("groups", po::value<int>(&groups)->default_value(-1), "Groups")
    ("collections", po::value<int>(&collections)->default_value(-1), "Collections")
    ("objs", po::value<int>(&objs)->default_value(-1), "Objs")
    ("policies", po::value<int>(&policies)->default_value(-1), "Policies")
    ("fullUsers", po::value<int>(&fullUsers)->default_value(-1), "Full Users")
    ("waitingUsers", po::value<int>(&waitingUsers)->default_value(-1), "Waiting Users")
    ("fullGroups", po::value<int>(&fullGroups)->default_value(-1), "Full Groups")
    ("waitingGroups", po::value<int>(&waitingGroups)->default_value(-1), "Waiting Groups")
    ("fullCollections", po::value<int>(&fullCollections)->default_value(-1), "Full Collections")
    ("waitingCollections", po::value<int>(&waitingCollections)->default_value(-1), "Waiting Collections")
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

  Log::config("Counts   ", logLevel);

   if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  
  Storage::instance()->init(dbConn, dbName);

  DataArray a;
  Data lines(a);
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

  if (collections >= 0) {
    lines.push_back({ 
      { "type", "collections" }, 
      { "expected", collections }
    });
  }
  if (fullCollections >= 0) {
    lines.push_back({ 
      { "type", "collections" }, 
      { "expected", fullCollections },
      { "full", true },
      { "field", "name" }
    });
  }
  if (waitingCollections >= 0) {
    lines.push_back({ 
      { "type", "collections" }, 
      { "expected", waitingCollections },
      { "waiting", true },
      { "field", "name" }
    });
  }

  if (objs >= 0) {
    lines.push_back({ 
      { "type", "objs" }, 
      { "expected", objs }
    });
  }

  if (policies >= 0) {
    lines.push_back({ 
      { "type", "policies" }, 
      { "expected", policies }
    });
  }

  bool success = true;
  for (auto l: lines) {
  
    Data o = l;
    Data q = {{}};
    
    auto type = o.getString("type");
    if (!type) {
      continue;
    }
    auto expected = o.getNumber("expected");
    if (!expected) {
      continue;
    }
    auto field = o.getString("field", true);
    if (Json::getBool(o, "full", true)) {
      q.setObj(field.value(), {
        { "$ne", "Waiting discovery" }
      });
    }
    else if (o.getBool("waiting", true)) {
      q.setString(field.value(), "Waiting discovery");
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
