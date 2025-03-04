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
#include "collectioni.hpp"

using namespace std;

namespace nodes {

class ResultImpl {

public:
#ifdef MONGO_DB
  ResultImpl(CollectionImpl coll, bsoncxx::document::view_or_value query, const vector<string> &fields, 
      optional<int> limit=nullopt, optional<bsoncxx::document::view_or_value> sort=nullopt): 
    _c(coll), _mc(coll._c), _q(query), _f(fields), _limit(limit), _sort(sort) {};
#else
  ResultImpl(CollectionImpl coll, const Data &query, const vector<string> &fields, 
      optional<int> limit=nullopt, optional<const Data> sort=nullopt): 
    _c(coll), _q(query), _f(fields), _limit(limit), _sort(sort) {};
#endif

  // public for testing.
  Data fixObjects(const Data &data);

  optional<Data> value();
  optional<Data> all();
  
private:

  CollectionImpl _c;
  vector<string> _f;
  optional<int> _limit;

#ifdef MONGO_DB
  mongocxx::collection _mc;
  bsoncxx::document::view_or_value _q;
  optional<bsoncxx::document::view_or_value> _sort;
  mongocxx::cursor find();
#else
  Data _q;
  optional<const Data> _sort;
#endif
};

} // nodes

#endif // H_resulti
