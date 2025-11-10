/*
  storage.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
    
  Storage for Nodes.
  
  Use like:
  
    auto doc = User(storage).find({{"name", "tracy"}}).one();
    auto alldocs = User(storage).find().all();

  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_storage
#define H_storage

#include "storage/result.hpp"
#include "storage/schema.hpp"
#include "data.hpp"
#include <map>

using namespace std;
using vops::DictV;

namespace nodes {

class StorageImpl;

class Storage {

public:
  static shared_ptr<Storage> instance() {
    if(!_instance) {
      _instance.reset(new Storage());
    }
    return _instance;
  };
    
  void init(const string &dbConn, const string &dbName, const string &schema="");
  
  shared_ptr<StorageImpl> _impl;

  bool bulkInsert(const string &collName, const vector<DictO> &objs);
    // Insert all of the objects into the collection with that name.

  DictO getNowO();
  Data getNow();
    // return the correct object for a date that is now.
    
  void collectionWasChanged(const string &name);
    // return the last date a collection was changed.
    
  long collectionChanged(const string &name) {
    return _changed[name];
  }
    // return the last date a collection was changed.
    
  void allCollectionsChanged();
    // everything changed.
    
  bool collName(const string &type, string *name, bool checkinternal=true);
    // get the collection name from the schema given a type.
    // if the collection doesn't exist in the schema or it is marked as "internal"
    // then this will return false
    
  bool parentInfo(const string &type, string *parentfield, optional<string *> parenttype = nullopt, optional<string *> namefield = nullopt);
    // if the type has a parent field, then return that.
    
  vector<DictO> _schema;
    // the loaded schema

  static bool appendArray(DictO *obj, const string &name, const string &val);
    // given the object and the name of a member that contains an array, find the val
    // in the array. If it's found then false is returned. otherwise the value
    // is appended to the array.
    
  static bool arrayHas(const DictO &obj, const string &name, const string &val);
    // Return true if the array has the value
    
private:

  // there can be only 1.
  Storage() {};
  static shared_ptr<Storage> _instance;
  
  map<string, long> _changed;
    // a map of collection names, along with when they were last changed.
    
  optional<DictO> searchSchema(const string &type);
  static bool hasArrayValue(const DictV &arr, const string &val);

};

} // nodes

#endif // H_storage
