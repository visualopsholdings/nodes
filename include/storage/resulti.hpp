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
  ResultImpl(mongocxx::collection coll, bsoncxx::document::view_or_value query, const vector<string> &fields): 
    _c(coll), _q(query), _f(fields) {};

  // public for testing.
  Data fixObjects(const Data &data);

  optional<Data> value();
  optional<Data> values();
  
  mongocxx::cursor find();
  
  vector<string> _f;
  mongocxx::collection _c;
  bsoncxx::document::view_or_value _q;
  
private:

  Data fixObject(const Data &j);

};

} // nodes

#endif // H_resulti
