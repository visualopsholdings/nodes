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

#ifdef MONGO_DB
#include <mongocxx/collection.hpp>
#endif
#include <string>

using namespace std;

namespace nodes {

class Data;

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
  optional<string> insert_one(const Data &doc);
  Data findAll(const Data &query, optional<const Data> &sort);
  Data find(const Data &query, optional<const Data> &sort);
#endif

  // public for testing.
  Data fixObjects(const Data &data);

private:
  Data fixObject(const Data &j);

#ifndef MONGO_DB
  string _name;
  string getCollectionFolder();
  bool match(const Data &json, const Data &query);
#endif
};

} // nodes

#endif // H_collectioni
