/*
  storage.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
    
  Storage for Nodes.
  
  Use like:
  
    auto doc = User(storage).find({{"name", "tracy"}}).value();
    auto alldocs = User(storage).find().values();

  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_storage
#define H_storage

#include "storage/result.hpp"
#include "storage/schema.hpp"

using namespace std;

class StorageImpl;

class Storage {

public:
  static shared_ptr<Storage> instance() {
    if(!_instance) {
      _instance.reset(new Storage());
    }
    return _instance;
  };
    
  void init(const string &dbConn, const string &dbName);
  
  shared_ptr<StorageImpl> _impl;

  bool bulkInsert(const string &collName, boost::json::array &objs);
    // Insert all of the objects into the collection with that name.

  json getNow();
    // return the correct JSON for a date that is now.
    
  void collectionWasChanged(const string &name);
    // return the last date a collection was changed.
    
  long collectionChanged(const string &name) {
    return _changed[name];
  }
    // return the last date a collection was changed.
    
  void allCollectionsChanged();
    // everything changed.
    
  string collName(const string &type, optional<string> coll);
  string collName(const string &type);
    // get the collection name from the schema given a type.
    
  optional<string> parentField(const string &type);
    // if the type has a parent field, then return that.
    
  boost::json::array _schema;
    // the loaded schema

private:

  // there can be only 1.
  Storage() {};
  static shared_ptr<Storage> _instance;
  
  map<string, long> _changed;
    // a map of collection names, along with when they were last changed.
    
};

#endif // H_storage
