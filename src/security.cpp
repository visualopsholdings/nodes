/*
  security.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Jun-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "security.hpp"

#include "vid.hpp"
#include "b64.hpp"
#include "storage/schema.hpp"
#include "storage/cursor.hpp"

#include <openssl/evp.h>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <bsoncxx/oid.hpp>
#include <boost/algorithm/string.hpp>

#define SHA1_LEN    128
#define ITERATIONS  12000

shared_ptr<Security> Security::_instance;

bool Security::valid(const VID &vid, const string &salt, const string &hash) {

  string password = vid.password();
  
  unsigned char out[SHA1_LEN+1];
  int len = PKCS5_PBKDF2_HMAC_SHA1((const char *)password.c_str(), password.length(), (const unsigned char *)salt.c_str(), salt.length(), ITERATIONS, SHA1_LEN, out);
  
  string dechash = B64::toBinary(hash);
  
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
  for (auto a: policy.value().at("accesses").as_array()) {
    for (auto u: a.at("users").as_array()) {
      addTo(users, u.as_string().c_str());
    }
    for (auto g: a.at("groups").as_array()) {
      addTo(&grps, g.as_string().c_str());
    }
  }
  
  // add all the users in all the groups.
  if (grps.size() > 0) {
    auto groups = Group().findByIds(grps, { "members" }).values();
    for (auto g: groups.value().as_array()) {
      for (auto u: g.at("members").as_array()) {
        addTo(users, u.at("user").as_string().c_str());
      }
    }
  }
  
}

void Security::getIndexes(Schema &schema, const string &id, vector<string> *ids) {

  auto indexes = schema.find({{ "_id", id }}, {"value"}).value();
  if (!indexes) {
    BOOST_LOG_TRIVIAL(error) << "indexes missing";
    return;
  }
  string s = boost::json::value_to<string>(indexes.value().at("value"));
  boost::split(*ids, s, boost::is_any_of(","));

}

boost::json::array Security::createArray(const vector<string> &list) {

  boost::json::array a;
  for (auto i: list) {
    a.push_back(boost::json::string(i));
  }
  return a;

}

void Security::queryIndexes(Schema &schema, const vector<string> &inids, vector<string> *ids) {

  json q = { { "_id", {{ "$in", createArray(inids) }}}};
  
  auto indexes = schema.find(q).values();
  if (!indexes) {
    BOOST_LOG_TRIVIAL(error) << "indexes missing";
    return;
  }
  for (auto i: indexes.value().as_array()) {
    vector<string> l;
    string s = boost::json::value_to<string>(i.at("value"));
    boost::split(l, s, boost::is_any_of(","));
    for (auto j: l) {
      addTo(ids, j);
    }
  }
  

}

optional<json> Security::with(Schema &schema, Schema &gperm, Schema &uperm, const string &userid, const json &query, const vector<string> &fields) {

  // collect all the groups the user is in.
  vector<string> glist;
  UserInGroups useringroups;
  getIndexes(useringroups, userid, &glist);
  
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
  
  return schema.find(q, fields).values();

}

optional<json> Security::withView(Schema &schema, const string &userid, const json &query, const vector<string> &fields) {

  GroupViewPermissions groupviews;
  UserViewPermissions userviews;
  return with(schema, groupviews, userviews, userid, query, fields);
  
}

optional<json> Security::withEdit(Schema &schema, const string &userid, const json &query, const vector<string> &fields) {

  GroupEditPermissions groupviews;
  UserEditPermissions userviews;
  return with(schema, groupviews, userviews, userid, query, fields);
  
}
