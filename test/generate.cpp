/*
  generate.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 22-Jan-2025
    
  Command line driver for generating objects
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage.hpp"
#include "json.hpp"
#include "log.hpp"
#include "date.hpp"

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
  int count;
  string collection;
  string collectionValue;
  string collectionField;
  string user;
  string policy;
  string date;
  
  po::options_description desc("Allowed options");
  desc.add_options()
    ("count", po::value<int>(&count)->default_value(5), "Count")
    ("collection", po::value<string>(&collection)->default_value("objs"), "Collection Name")
    ("collectionField", po::value<string>(&collectionField)->default_value("coll"), "Parent Collection Field Name")
    ("collectionValue", po::value<string>(&collectionValue)->required(), "Parent Collection Value")
    ("user", po::value<string>(&user)->required(), "User")
    ("policy", po::value<string>(&policy)->required(), "Policy")
    ("date", po::value<string>(&date)->required(), "Start Date (every 5 minutes)")
    ("dbConn", po::value<string>(&dbConn)->default_value("mongodb://127.0.0.1:27017"), "DB Connection string.")
    ("dbName", po::value<string>(&dbName)->default_value("test"), "DB name.")
    ("logLevel", po::value<string>(&logLevel)->default_value("info"), "Logging level [trace, debug, warn, info].")
    ("help", "produce help message")
    ;
  po::positional_options_description p;
  p.add("json", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);
  po::notify(vm);   

  Log::config("Generate   ", logLevel);

   if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }
  
  Storage::instance()->init(dbConn, dbName);

  stringstream ds;
  ds << date << "T00:00.000+0000";
  long d = Date::fromISODate(ds.str());
  
  for (int i=1; i<count+1; i++) {
    d += 5 * 60 * 1000;
    stringstream text;
    text << "Obj " << i;
    Data obj = {
      { collectionField, collectionValue },
      { "policy", policy },
      { "user", user },
      { "modifyDate", { { "$date", d } } },
      { "text", text.str() }
    };
    auto res = SchemaImpl::insertGeneral(collection, obj);
    if (!res) {
      L_ERROR("failed to insert");
      return 1;
    }
    if ((i % 10000) == 0) {
      cout << "." << flush;
    }
  }
  cout << endl;
  L_INFO("Inserted: " << count);
  
  return 0;
  
}
