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
  Schema(Schema &schema) : _storage(schema._storage) {}

  void deleteMany(const json &doc);
    // delete all documents that match the query.
    
  optional<string> insert(const json &doc);
    // insert a new document.
    
  Cursor find(const json &query, const vector<string> &fields = {});
    // find documents with the query.
    
  Cursor findById(const string &id, const vector<string> &fields = {});
    // find the document with this id.
    
  Cursor findByIds(const vector<string> &ids, const vector<string> &fields = {});
    // find the document with these ids.
    
  void aggregate(const string &filename);
    // process an aggregatin pipeline in the file.

  virtual string collName() = 0;
  
private:
    
  Storage &_storage;
  
};

class User: public Schema {

public:
  User(Storage &storage): Schema(storage) {}
  User(Schema &schema): Schema(schema) {}
  
  virtual string collName() { return "users"; };

};

class Policy: public Schema {

public:
  Policy(Storage &storage): Schema(storage) {}
  Policy(Schema &schema): Schema(schema) {}
  
  virtual string collName() { return "policies"; };

};

class Stream: public Schema {

public:
  Stream(Storage &storage): Schema(storage) {}
  Stream(Schema &schema): Schema(schema) {}
  
  virtual string collName() { return "streams"; };

};

class Idea: public Schema {

public:
  Idea(Storage &storage): Schema(storage) {}
  Idea(Schema &schema): Schema(schema) {}
  
  virtual string collName() { return "ideas"; };

};

class Group: public Schema {

public:
  Group(Storage &storage): Schema(storage) {}
  Group(Schema &schema): Schema(schema) {}
  
  virtual string collName() { return "groups"; };
  
};

class UserInGroups: public Schema {

public:
  UserInGroups(Storage &storage): Schema(storage) {}
  UserInGroups(Schema &schema): Schema(schema) {}
  
  virtual string collName() { return "useringroups"; };
  
};

class GroupViewPermissions: public Schema {

public:
  GroupViewPermissions(Storage &storage): Schema(storage) {}
  GroupViewPermissions(Schema &schema): Schema(schema) {}
  
  virtual string collName() { return "groupviewpermissions"; };
  
};

class GroupEditPermissions: public Schema {

public:
  GroupEditPermissions(Storage &storage): Schema(storage) {}
  GroupEditPermissions(Schema &schema): Schema(schema) {}
  
  virtual string collName() { return "groupeditpermissions"; };
  
};

class UserViewPermissions: public Schema {

public:
  UserViewPermissions(Storage &storage): Schema(storage) {}
  UserViewPermissions(Schema &schema): Schema(schema) {}
  
  virtual string collName() { return "userviewpermissions"; };
  
};

class UserEditPermissions: public Schema {

public:
  UserEditPermissions(Storage &storage): Schema(storage) {}
  UserEditPermissions(Schema &schema): Schema(schema) {}
  
  virtual string collName() { return "usereditpermissions"; };
  
};

#endif // H_schema
