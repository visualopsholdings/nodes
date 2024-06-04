/*
  storage.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
    
  Storage for ZMQChat
  
  Use like:
  
    auto doc = storage.coll("users").find({{"name", "tracy"}}).value();
    auto alldocs = storage.coll("users").find().values();

  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_storage
#define H_storage

#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

class CursorImpl;

class Cursor {

public:
  Cursor(shared_ptr<CursorImpl> impl): _impl(impl) {};
  
  optional<json> value();
    // return the value of the query as a JSON object.
    
  optional<json> values();
    // return the value of the query as a JSON array.
    
private:
  shared_ptr<CursorImpl> _impl;
  
};

class CollectionImpl;

class Collection {

public:
  Collection(shared_ptr<CollectionImpl> impl): _impl(impl) {};
  Collection() {};
  
  Cursor find(const json &query);
    // find something, returning a cursor.
    
  Cursor find();
    // find everything, returning a cursor.
  
private:
  shared_ptr<CollectionImpl> _impl;
  
};

class StorageImpl;

class Storage {

public:
  Storage();
  
  Collection coll(const string &name);
    // get the collection with the name.
    
private:
  shared_ptr<StorageImpl> _impl;
  
};

#endif // H_storage
