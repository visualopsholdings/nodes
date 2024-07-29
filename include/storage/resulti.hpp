/*
  resulti.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  DB result implementation for Nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_resulti
#define H_resulti

#include <mongocxx/collection.hpp>
#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

class ResultImpl {

public:
  ResultImpl(mongocxx::collection coll, bsoncxx::document::view_or_value query, const vector<string> &fields): 
    _c(coll), _q(query), _f(fields) {};

  // public for testing.
  json fixObjects(const json &json);

  optional<json> value();
  optional<boost::json::array> values();
  
//private:
//  friend class Result;
  
  mongocxx::cursor find();
  
  vector<string> _f;
  mongocxx::collection _c;
  bsoncxx::document::view_or_value _q;
  
};


#endif // H_resulti
