/*
  collectioni.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  Collection implemementation for Nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_collectioni
#define H_collectioni

#include "dict.hpp"

#ifdef MONGO_DB
#include <mongocxx/collection.hpp>
#endif
#include <string>

using namespace std;
using vops::DictO;
using vops::DictG;
using vops::DictV;

namespace nodes {

class CollectionImpl {

public:
  
#ifdef MONGO_DB
  CollectionImpl(mongocxx::collection c): _c(c) {};
#else
  CollectionImpl(const string &name): _name(name) {};
#endif
  CollectionImpl() {};

#ifdef MONGO_DB
  mongocxx::collection _c;
#else
  optional<string> insert_one(const DictO &doc);
  void delete_many(const DictO &doc);
  DictV findAll(const DictO &query, optional<const DictO> &sort);
  DictO find(const DictO &query, optional<const DictO> &sort);
  DictV findByIds(const vector<string> &ids);
#endif

  // public for testing.
  DictO fixObjects(const DictO &data);

  static string toISODate(const DictO &date);
    // convert dates correctly.
    
private:
  DictO fixObject(const DictO &j);

#ifndef MONGO_DB
  string _name;
  string getCollectionFolder();
  bool match(const DictO &doc, const DictO &query);
#endif
};

} // nodes

#endif // H_collectioni
