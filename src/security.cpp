/*
  security.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Jun-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "security.hpp"

#include "vid.hpp"
#include "storage.hpp"

#include <openssl/evp.h>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <bsoncxx/oid.hpp>
#include <boost/algorithm/string.hpp>
#include <base64.hpp>
#include <sstream>

#define SHA1_LEN    128
#define ITERATIONS  12000

shared_ptr<Security> Security::_instance;

bool Security::valid(const VID &vid, const string &salt, const string &hash) {

  string password = vid.password();
  
  unsigned char out[SHA1_LEN+1];
  int len = PKCS5_PBKDF2_HMAC_SHA1((const char *)password.c_str(), password.length(), (const unsigned char *)salt.c_str(), salt.length(), ITERATIONS, SHA1_LEN, out);
  
  string dechash = base64::from_base64(hash);
  
  for (int i=0; i<SHA1_LEN; i++) {
    if ((unsigned char)dechash[i] != out[i]) {
      return false;
    }
  }
  
  return true;
  
}

void Security::addTo(vector<string> *v, const string &val) {

  if (find(v->begin(), v->end(), val) == v->end()) {
    v->push_back(val);
  }

}

void Security::getPolicyUsers(const string &id, vector<string> *users) {

  // This is so much simpler than the Visual Ops version which determines the role of
  // each user.
  
  auto policy = Policy().findById(id, { "accesses" }).value();
  if (!policy) {
    BOOST_LOG_TRIVIAL(error) << "policy missing";
    return;
  }

  // add all the users directly referenced and collect the groups.
  vector<string> grps;
  for (auto a: policy.value().accesses()) {
    for (auto u: a.users()) addTo(users, u);
    for (auto g: a.groups()) addTo(&grps, g);
  }
 
  // add all the users in all the groups.
  if (grps.size() > 0) {
    auto groups = Group().findByIds(grps, { "members" }).values();
    for (auto g: groups.value()) {
      for (auto u: g.members()) addTo(users, u.user());
    }
  }

}

boost::json::array Security::createArray(const vector<string> &list) {

  boost::json::array a;
  for (auto i: list) {
    a.push_back(boost::json::string(i));
  }
  return a;

}

void Security::queryIndexes(Schema<IndexRow> &schema, const vector<string> &inids, vector<string> *ids) {

  json q = { { "_id", {{ "$in", createArray(inids) }}}};
  
  auto indexes = schema.find(q).values();
  if (!indexes) {
    BOOST_LOG_TRIVIAL(trace) << "indexes missing for " << schema.collName();
    return;
  }
  for (auto i: indexes.value()) {
    for (auto j: i.values()) {
      addTo(ids, j);
    }
  }
  

}

json Security::withQuery(Schema<IndexRow> &gperm, Schema<IndexRow> &uperm, const string &userid, const json &query) {

  // collect all the groups the user is in.
  UserInGroups useringroups;
  auto indexes = useringroups.find({{ "_id", userid }}, {"value"}).value();
  vector<string> glist;
  if (indexes) {
    glist = indexes.value().values();
  }
  
  // collect all the policies for those groips
  vector<string> plist;
  queryIndexes(gperm, glist, &plist);
  
  // add all the policies just for this user.
  queryIndexes(uperm, { userid }, &plist);

  json q = { { "$and", { 
    query,
    { { "policy", { { "$in", createArray(plist) } } } }
  } } };
  BOOST_LOG_TRIVIAL(trace) << q;

  return q;
  
}

boost::json::object Security::makeLine(const string &type, int access, const string &name, const vector<string> &ids, int index) {

  // Lines have this simple format so they can be easily edited in a visual editor.

  boost::json::object line;
  
  stringstream ss;
  ss << "//accesses/" << access << "/" << type << "s/" << index;
  line["path"] = ss.str();
  line["type"] = name;
  line["context"] = type;
  BOOST_LOG_TRIVIAL(trace) << ids[index];
  if (type == "user") {
    auto user = User().findById(ids[index], { "fullname" }).value();
    line["name"] = !user ? "???" : user.value().fullname();
  }
  else {
    auto group = Group().findById(ids[index], { "name" }).value();
    line["name"] = !group ? "???" : group.value().name();
  }
  return line;
  
}

optional<json> Security::getPolicyLines(const string &id) {

  auto policy = Policy().findById(id, { "accesses" }).value();
  if (!policy) {
    BOOST_LOG_TRIVIAL(error) << "policy missing";
    return nullopt;
  }
  
  boost::json::array lines;
  auto accesses = policy.value().accesses();
  for (int i=0; i<accesses.size(); i++) {
  
    auto users = accesses[i].users();
    for (int j=0; j<users.size(); j++) {
       lines.push_back(makeLine("user", i, accesses[i].name(), users, j));
    }
    
    auto groups = accesses[i].groups();
    for (int j=0; j<groups.size(); j++) {
      lines.push_back(makeLine("group", i, accesses[i].name(), groups, j));
    }
  }
  
  return lines;
  
}

optional<string> Security::findPolicyForUser(const string &id) {
  
  boost::json::array empty;
  json q = { 
    { "accesses.0.name", "view" },
    { "accesses.0.groups", empty }, 
    { "accesses.0.users",  { id } }, 
    { "accesses.1.name", "edit" },
    { "accesses.1.groups", empty }, 
    { "accesses.1.users",  { id } }, 
    { "accesses.2.name", "exec" },
    { "accesses.2.groups", empty }, 
    { "accesses.2.users",  { id } }
	};
  
  auto policy = Policy().find(q).value();
  if (!policy) {
      auto newpolicy = Policy().insert({
      { "accesses", {
        { { "name", "view" }, 
          { "groups", empty },
          { "users", { id } }
          },
        { { "name", "edit" }, 
          { "groups", empty },
          { "users", { id } }
          },
        { { "name", "exec" }, 
          { "groups", empty },
          { "users", { id } }
          }
        } 
      },
      { "modifyDate", Storage::instance()->getNow() }
    });
    if (!newpolicy) {
      BOOST_LOG_TRIVIAL(error) << "could not create new policy";
    }
    return newpolicy;
  }
  
  return policy.value().id();
  
}
