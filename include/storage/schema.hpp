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

#include <set>

namespace nodes {

class Data;

template <typename RowType>
class Schema: public SchemaImpl {

public:

  // find documents with the query.
  Result<RowType> find(const Data &query, const vector<string> &fields = {}) {
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
  UserRow(Data data): DynamicRow(data) {}
  
  string name() { return getString("name"); }
  string fullname() { return getString("fullname"); }
  string salt() { return getString("salt"); }
  string hash() { return getString("hash"); }
  bool admin() { return getBool("admin"); }
  bool active() { return getBool("active"); }
  string modifyDate() { return getString("modifyDate"); }
  bool upstream() { return getBool("upstream", true); }
  bool deleted() { return getBool("deleted", true); }
  
};

class User: public Schema<UserRow> {

public:
  
  virtual string collName() { return "users"; };

};

class AccessRow: public DynamicRow {

public:
  AccessRow(Data data): DynamicRow(data) {}
  
  string name() { return getString("name"); }
  vector<string> users() { return getStringArray("users"); }
  vector<string> groups() { return getStringArray("groups"); }
  
};

class PolicyRow: public DynamicRow {

public:
  PolicyRow(Data data): DynamicRow(data) {}
  
  vector<AccessRow> accesses();
  
};

class Policy: public Schema<PolicyRow> {

public:
  
  virtual string collName() { return "policies"; };

};

class MemberRow: public DynamicRow {

public:
  MemberRow(Data data): DynamicRow(data) {}
  
  string user() { return getString("user"); }
  
};

class GroupRow: public DynamicRow {

public:
  GroupRow(Data data): DynamicRow(data) {}
  
  string name() { return getString("name"); }
  string policy() { return getString("policy"); }
  string modifyDate() { return getString("modifyDate"); }
  vector<MemberRow> members();
  bool upstream() { return getBool("upstream", true); }
  bool deleted() { return getBool("deleted", true); }
  
};

class Group: public Schema<GroupRow> {

public:
  
  virtual string collName() { return "groups"; };
  
  bool addMember(const string &group, const string &user);
  bool removeMember(const string &group, const string &user);
  
private:
  bool getMemberSet(const string &group, set<string> *mset);
  bool saveMemberSet(const string &group, const set<string> &mset);

};

class InfoRow: public DynamicRow {

public:
  InfoRow(Data data): DynamicRow(data) {}
  
  string type() const { return getString("type"); }
  string text() const { return getString("text"); }
  
};

class Info: public Schema<InfoRow> {

public:
  
  virtual string collName() { return "infos"; };
  
  static optional<string> getInfo(const vector<InfoRow> &infos, const string &type);
  static string getInfoSafe(optional<vector<InfoRow> > infos, const string &type, const string &def);

};

class SiteRow: public DynamicRow {

public:
  SiteRow(Data data): DynamicRow(data) {}
  
  string headerTitle() const { return getString("headerTitle"); }
  string streamBgColor() const { return getString("streamBgColor"); }

};

class Site: public Schema<SiteRow> {

public:
  
  virtual string collName() { return "sites"; };
  
};

class NodeRow: public DynamicRow {

public:
  NodeRow(Data data): DynamicRow(data) {}
  
  string serverId() const { return getString("serverId"); }
  string pubKey() const { return getString("pubKey"); }
  bool valid() const { return getBool("valid"); }
  
};

class Node: public Schema<NodeRow> {

public:
  
  virtual string collName() { return "nodes"; };
  
};

class IndexRow: public DynamicRow {

public:
  IndexRow(Data data): DynamicRow(data) {}
  
  vector<string> all();
  
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

} // nodes

#endif // H_schema
