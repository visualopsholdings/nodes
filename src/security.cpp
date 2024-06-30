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
#include "storage.hpp"

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
    BOOST_LOG_TRIVIAL(error) << "indexes missing";
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
