/*
  schema.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 30-June-2024
    
  A typed schema for Nodes.

  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_schema
#define H_schema

#include "schemai.hpp"

template <typename RowType>
class Schema: public SchemaImpl {

public:

  // find documents with the query.
  Result<RowType> find(const json &query, const vector<string> &fields = {}) {
    return Result<RowType>(findResult(query, fields));
  }
    
  // find the document with this id.
  Result<RowType> findById(const string &id, const vector<string> &fields = {}) {
    return Result<RowType>(findByIdResult(id, fields));
  }
  
  // find the document with these ids.
  Result<RowType> findByIds(const vector<string> &ids, const vector<string> &fields = {}) {
    return Result<RowType>(findByIdsResult(ids, fields));
  }

};

class UserRow: public DynamicRow {

public:
  UserRow(json json): DynamicRow(json) {}
  
  string id() { return getString("id"); }
  string name() { return getString("name"); }
  string fullname() { return getString("fullname"); }
  string salt() { return getString("salt"); }
  string hash() { return getString("hash"); }
  bool admin() { return getBool("admin"); }
  
};

class User: public Schema<UserRow> {

public:
  
  virtual string collName() { return "users"; };

};

class AccessRow: public DynamicRow {

public:
  AccessRow(json json): DynamicRow(json) {}
  
  vector<string> users() { return getStringArray("users"); }
  vector<string> groups() { return getStringArray("groups"); }
  
};

class PolicyRow: public DynamicRow {

public:
  PolicyRow(json json): DynamicRow(json) {}
  
  vector<AccessRow> accesses();
  
};

class Policy: public Schema<PolicyRow> {

public:
  
  virtual string collName() { return "policies"; };

};

class StreamRow: public DynamicRow {

public:
  StreamRow(json json): DynamicRow(json) {}

};

class Stream: public Schema<StreamRow> {

public:
  
  virtual string collName() { return "streams"; };

};

class IdeaRow: public DynamicRow {

public:
  IdeaRow(json json): DynamicRow(json) {}

};

class Idea: public Schema<IdeaRow> {

public:
  
  virtual string collName() { return "ideas"; };

};

class MemberRow: public DynamicRow {

public:
  MemberRow(json json): DynamicRow(json) {}
  
  string user() { return getString("user"); }
  
};

class GroupRow: public DynamicRow {

public:
  GroupRow(json json): DynamicRow(json) {}
  
  vector<MemberRow> members();
  
};

class Group: public Schema<GroupRow> {

public:
  
  virtual string collName() { return "groups"; };
  
};

class IndexRow: public DynamicRow {

public:
  IndexRow(json json): DynamicRow(json) {}
  
  vector<string> values();
  
};

class UserInGroups: public Schema<IndexRow> {

public:
  
  virtual string collName() { return "useringroups"; };
  
};

class GroupViewPermissions: public Schema<IndexRow> {

public:
  
  virtual string collName() { return "groupviewpermissions"; };
  
};

class GroupEditPermissions: public Schema<IndexRow> {

public:
  
  virtual string collName() { return "groupeditpermissions"; };
  
};

class UserViewPermissions: public Schema<IndexRow> {

public:
  
  virtual string collName() { return "userviewpermissions"; };
  
};

class UserEditPermissions: public Schema<IndexRow> {

public:
  
  virtual string collName() { return "usereditpermissions"; };
  
};

#endif // H_schema
