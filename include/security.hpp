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

#include <string>
#include <boost/json.hpp>
#include <memory>
#include <tuple>

using namespace std;
using json = boost::json::value;

class VID;

typedef tuple<string, string, string> addTupleType;
    // the add tuple is "type", "class" and "id"
    // type: "view" | "edit" | "exec"
    // context: "user" | "group"

class Security {

public:

  static shared_ptr<Security> instance() {
    if(!_instance) {
      _instance.reset(new Security());
    }
    return _instance;
  };
    
  bool valid(const VID &vid, const string &salt, const string &hash);
    // return true if the VID is valid. The salt is generated with the user, and the
    // hash is a premade hash of the password (we don't store the actual password)
    
  void getPolicyUsers(const string &id, vector<string> *users);
    // get a list of users that are in this policy.
  
  // execute a query ensuring that the user can view the results.
  template <typename RowType>
  Result<RowType> withView(Schema<RowType> &schema, optional<string> me, const json &query, const vector<string> &fields = {}) {
  
    if (me) {
      GroupViewPermissions groupviews;
      UserViewPermissions userviews;
      return schema.find(withQuery(groupviews, userviews, me.value(), query), fields);
    }

    return schema.find(query, fields);    
    
  }

  // execute a query ensuring that the user can edit the results.
  template <typename RowType>
  Result<RowType> withEdit(Schema<RowType> &schema, optional<string> me, const json &query, const vector<string> &fields = {}) {
  
    if (me) {
      GroupEditPermissions groupedits;
      UserEditPermissions useredits;
      return schema.find(withQuery(groupedits, useredits, me.value(), query), fields);
    }

    return schema.find(query, fields);    
    
  }

  template <typename RowType>
  bool canEdit(Schema<RowType> &schema, optional<string> me, const string &id) {
  
    if (me) {
      GroupEditPermissions groupedits;
      UserEditPermissions useredits;
      return schema.find(withQuery(groupedits, useredits, me.value(), json{ { "_id", { { "$oid", id } } } })).value() != nullopt;
    }
      
    return true;
    
  }
    
  optional<json> getPolicyLines(const string &id);
    // get a json array of policy lines.
  
  optional<string> findPolicyForUser(const string &userid);
    // find the policy for this user, if it doesn't exist create it.
    
  optional<string> modifyPolicy(const string &id, const vector<addTupleType> &add, const vector<string> &remove);
    // find the policy for this user, if it doesn't exist create it.
    
private:

  // there can be only 1.
  Security() {};
  static shared_ptr<Security> _instance;
  
  void addTo(vector<string> *v, const string &val);
  void queryIndexes(Schema<IndexRow> &schema, const vector<string> &inids, vector<string> *ids);
  boost::json::array createArray(const vector<string> &list);
  json withQuery(Schema<IndexRow> &gperm, Schema<IndexRow> &uperm, const string &userid, const json &query);
  boost::json::object makeLine(const string &type, int access, const string &name, const vector<string> &ids, int index);
  void removeAt(json *obj, const string &fullpath);
  void addPolicy(json *obj, const string &type, const string &context, const string &id);
  json policyToQuery(const json &obj);

};

#endif // H_security
