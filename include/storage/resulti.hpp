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

#include "data.hpp"

#include <mongocxx/collection.hpp>

using namespace std;

namespace nodes {

class ResultImpl {

public:
  ResultImpl(mongocxx::collection coll, bsoncxx::document::view_or_value query, const vector<string> &fields, 
      optional<int> limit=nullopt, optional<bsoncxx::document::view_or_value> sort=nullopt): 
    _c(coll), _q(query), _f(fields), _limit(limit), _sort(sort) {};

  // public for testing.
  Data fixObjects(const Data &data);

  optional<Data> value();
  optional<Data> all();
  
  mongocxx::cursor find();
  
  vector<string> _f;
  mongocxx::collection _c;
  bsoncxx::document::view_or_value _q;
  optional<int> _limit;
  optional<bsoncxx::document::view_or_value> _sort;

private:

  Data fixObject(const Data &j);

};

} // nodes

#endif // H_resulti
