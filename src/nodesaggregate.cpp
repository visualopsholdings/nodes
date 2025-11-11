/*
  nodesaggregate.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 25-Jun-2024
    
  Command line driver for indexing the collections using an aggregation pipeline.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage.hpp"
#include "storage/schemai.hpp"
#include "log.hpp"

#include <iostream>
#include <boost/program_options.hpp> 
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <fstream>

using namespace std;
namespace po = boost::program_options;
using namespace nodes;

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

  Log::config("nodesaggregate", logLevel);

  L_DEBUG("aggregating collection " << coll);
  
  Storage::instance()->init(dbConn, dbName);
  
  if (coll == "group") {
    Group().aggregate(filename);
  }
  else if (coll == "policy") {
    Policy().aggregate(filename);
  }
  else {
    L_ERROR("unknown collection " << coll);
  }
}
