/*
  schema.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  DB Schemas for ZMQChat.

  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_schema
#define H_schema

#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

class Storage;
class Cursor;

class Schema {

public:
  Schema(Storage &storage) : _storage(storage) {}

  void deleteMany(const json &doc);
    // delete all documents that match the query.
    
  optional<string> insert(const json &doc);
    // insert a new document.
    
  Cursor find(const json &query, const vector<string> &fields = {});
    // find documents with the query.
    
  Cursor find(const string &id, const vector<string> &fields = {});
    // find the document with this id.
    
  virtual string collName() = 0;
  
private:
  Storage &_storage;
  
};

class User: public Schema {

public:
  User(Storage &storage): Schema(storage) {}
  
  virtual string collName() { return "users"; };

};

class Policy: public Schema {

public:
  Policy(Storage &storage): Schema(storage) {}
  
  virtual string collName() { return "policies"; };

};

class Stream: public Schema {

public:
  Stream(Storage &storage): Schema(storage) {}
  
  virtual string collName() { return "streams"; };

};

#endif // H_schema
