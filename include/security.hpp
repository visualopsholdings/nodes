/*
  security.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Jun-2024
    
  Security code for Nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_security
#define H_security

#include "storage/schema.hpp"
#include "dict.hpp"

#include <string>
#include <memory>
#include <tuple>

using namespace std;
using vops::DictO;
using vops::DictV;

namespace nodes {

class VID;

typedef tuple<string, string, string> addTupleType;
    // the add tuple is "type", "class" and "id"
    // type: "view" | "edit" | "exec"
    // context: "user" | "group"

class Security {

public:

  static shared_ptr<Security> instance() {
    if (!_instance) {
      rebuild();
    }
    return _instance;
  };
    
  static void rebuild() {
    _instance.reset(new Security());
  }
  
  bool valid(const VID &vid, const string &salt, const string &hash);
    // return true if the VID is valid. The salt is generated with the user, and the
    // hash is a premade hash of the password (we don't store the actual password)
    
  void getPolicyUsers(const string &id, vector<string> *users);
    // get a list of users that are in this policy.
  
  void getPolicyGroups(const string &id, vector<string> *groups);
    // get a list of groups that are in this policy.
  
  Result<DynamicRow> withView(const string &collection, optional<string> me, const Data &query, const vector<string> &fields = {});
  Result<DynamicRow> withView(const string &collection, optional<string> me, const DictO &query, const vector<string> &fields = {}) {
    return withView(collection, me, Data(query), fields);
  }
    // execute a query ensuring that the user can view the results, using the name of the collection.

  Result<DynamicRow> withEdit(const string &collection, optional<string> me, const Data &query, const vector<string> &fields = {});
  Result<DynamicRow> withEdit(const string &collection, optional<string> me, const DictO &query, const vector<string> &fields = {}) {
    return withEdit(collection, me, Data(query), fields);
  }
    //execute a query ensuring that the user can edit the results, using the name of the collection.

  bool canEdit(const string &collection, optional<string> me, const string &id);
    // an object can be edited.
    
  // execute a query ensuring that the user can view the results.
  template <typename RowType>
  Result<RowType> withView(Schema<RowType> &schema, optional<string> me, const Data &query, const vector<string> &fields = {}) {
  
    if (me) {
      GroupViewPermissions groupviews;
      UserViewPermissions userviews;
      return schema.find(withQuery(groupviews, userviews, me.value(), query), fields);
    }

    return schema.find(query, fields);    

  }
  template <typename RowType>
  Result<RowType> withView(Schema<RowType> &schema, optional<string> me, const DictO &query, const vector<string> &fields = {}) {
    return withView(schema, me, Data(query), fields);
  }
  
  template <typename RowType>
  bool canEdit(Schema<RowType> &schema, optional<string> me, const string &id) {
    return canEdit(schema.collName(), me, id);
  }
    
  // Users can always be edited and viewed
  Result<User> withView(Schema<User> &schema, optional<string> me, const Data &query, const vector<string> &fields = {}) {
    return schema.find(query, fields);    
  }
  bool canEdit(Schema<User> &schema, optional<string> me, const string &id) {
    return true;
  }
  
  optional<Data> getPolicyLines(const string &id);
    // get an array of policy lines.
  
  optional<string> findPolicyForUser(const string &userid);
    // find the policy for this user, if it doesn't exist create it.
    
  optional<string> modifyPolicy(const string &id, const vector<addTupleType> &add, const vector<string> &remove);
    // find the policy for this user, if it doesn't exist create it.
    
  void regenerate();
    // run all the aggregators needed for security.

  void regenerateGroups();
    // run aggregator when group members change.
 
  optional<string> generateShareLink(const string &me, const string &urlprefix, 
                    const string &coll, const string &objid, const string &groupid, 
                    int expires, const string &bitsfield);
    // generate a link that can be passed to somebody, putting them in a team and only
    // for expires ours.
    
  optional<string> createShareToken(const string &collid, const string &me, const string &options, const string &groupid, const string &expires);
  optional<Data> expandShareToken(const string &token);
    // public for testing.
  
  string newSalt();
    // generate a new user salt value.
    
  string newPassword();
    // generate a new user password
    
  string newHash(const string &password, const string &salt);
    // generate a new user salt value.
    
  Data policyToQuery(const Data &obj);
    // convert a policy to a query for that policy.
    
private:

  // there can be only 1.
  Security();
  static shared_ptr<Security> _instance;
  
  string _key;
  string _iv;

  void addTo(vector<string> *v, const string &val);
  void queryIndexes(Schema<IndexRow> &schema, const vector<string> &inids, vector<string> *ids);
  DictV createArray(const vector<string> &list);
  Data createArray2(const vector<string> &list);
  Data withQuery(Schema<IndexRow> &gperm, Schema<IndexRow> &uperm, const string &userid, const Data &query);
  Data makeLine(const string &type, int access, const string &name, const vector<string> &ids, int index);
//  void removeAt(json *obj, const string &fullpath);
//  void addPolicy(Data *obj, const string &type, const string &context, const string &id);
  bool isValidId(const string &id);
  
};

} // nodes

#endif // H_security
