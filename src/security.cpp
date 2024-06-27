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

#define SHA1_LEN    128
#define ITERATIONS  12000

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

void Security::getPolicyUsers(Storage &storage, const string &id, vector<string> *users) {

  auto policy = Policy(storage).findById(id, { "accesses" }).value();
  if (!policy) {
    BOOST_LOG_TRIVIAL(error) << "policy missing";
    return;
  }
  BOOST_LOG_TRIVIAL(trace) << policy.value();
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
    auto groups = Group(storage).findByIds(grps, { "members" }).values();
    for (auto g: groups.value().as_array()) {
      for (auto u: g.at("members").as_array()) {
        addTo(users, u.at("user").as_string().c_str());
      }
    }
  }
  
}
