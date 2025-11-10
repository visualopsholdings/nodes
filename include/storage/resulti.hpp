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

#include "collectioni.hpp"
#include "dict.hpp"

using namespace std;
using vops::DictG;
using vops::DictO;
using vops::DictV;

namespace nodes {

class ResultImpl {

public:
#ifdef MONGO_DB
  ResultImpl(CollectionImpl coll, bsoncxx::document::view_or_value query, const vector<string> &fields, 
      optional<int> limit, optional<bsoncxx::document::view_or_value> sort): 
    _c(coll), _mc(coll._c), _q(query), _f(fields), _limit(limit), _sort(sort) {};
#else
  ResultImpl(CollectionImpl coll, const vector<string> &ids, const vector<string> &fields): 
    _c(coll), _ids(ids), _f(fields) {};
  ResultImpl(CollectionImpl coll, const DictO &query, const vector<string> &fields, 
      optional<int> limit, optional<const DictO> sort): 
    _c(coll), _q(query), _f(fields), _limit(limit), _sort(sort) {};
#endif

  // public for testing.
  DictG fixObjects(const DictG &data);

  optional<DictO> value();
  optional<DictV> all();
  
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
  DictO _q;
  optional<vector<string> > _ids;
  optional<const DictO> _sort;
#endif
};

} // nodes

#endif // H_resulti
