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
#include "json.hpp"
#include "encrypter.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <bsoncxx/oid.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/filesystem.hpp>
#include <base64.hpp>
#include <sstream>
#include <cstdlib>
#include <wordexp.h>
#include <ctime>
#include <iomanip>

#define SHA1_LEN    128
#define ITERATIONS  12000

shared_ptr<Security> Security::_instance;

Security::Security() {

  auto infos = Info().find({{ "type", { { "$in", {"tokenKey", "tokenIV"}}} }}, {"type", "text"}).values();
  if (!infos) {
    BOOST_LOG_TRIVIAL(error) << "missing infos";
    return;
  }
  auto key = Info::getInfo(infos.value(), "tokenKey");
  if (!key) {
    BOOST_LOG_TRIVIAL(error) << "missing key info";
    return;
  }
  _key = key.value();
  auto iv = Info::getInfo(infos.value(), "tokenIV");
  if (!iv) {
    BOOST_LOG_TRIVIAL(error) << "missing iv info";
    return;
  }
  _iv = key.value();
  BOOST_LOG_TRIVIAL(trace) << "key " << _key.length();
  BOOST_LOG_TRIVIAL(trace) << "iv " << _iv.length();
}

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

void Security::getPolicyGroups(const string &id, vector<string> *groups) {

  auto policy = Policy().findById(id, { "accesses" }).value();
  if (!policy) {
    BOOST_LOG_TRIVIAL(error) << "policy missing";
    return;
  }

  // collect the groups.
  vector<string> grps;
  for (auto a: policy.value().accesses()) {
    for (auto g: a.groups()) addTo(groups, g);
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
  line["id"] = ids[index];
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

optional<string> Security::findPolicyForUser(const string &userid) {
  
  boost::json::array empty;
  json obj = {
    { "accesses", {
      { { "name", "view" }, 
        { "groups", empty },
        { "users", { userid } }
        },
      { { "name", "edit" }, 
        { "groups", empty },
        { "users", { userid } }
        },
      { { "name", "exec" }, 
        { "groups", empty },
        { "users", { userid } }
        }
      } 
    },
    { "modifyDate", Storage::instance()->getNow() }
  };
    
  auto policy = Policy().find(policyToQuery(obj)).value();
  if (!policy) {
    auto newpolicy = Policy().insert(obj);
    if (!newpolicy) {
      BOOST_LOG_TRIVIAL(error) << "could not create new policy";
    }
    return newpolicy;
  }
  
  return policy.value().id();
  
}

void Security::removeAt(json *obj, const string &fullpath) {

  // this is the code that was needed in NodeJS to do this:
  // var spdb = spahql.db(policy);
  //   _.forEach(changes.remove, function(r) {
  //     spdb.select(r).destroyAll();
  //   });
  // this is VERY similar to https://datatracker.ietf.org/doc/html/rfc6901
  // except for the extra "/" at the start of each line
  
  // split the path into the start bit and the very last element, the 
  // last being a numeric index.
  auto res = fullpath.rfind("/");
  if (res == string::npos) {
    BOOST_LOG_TRIVIAL(warning) << "remove doesn't have correct syntax " << fullpath;
    return;
  }
  
  string path = fullpath.substr(0, res);
  if (path.size() > 1) {
    // remove the extra "/"
    path = path.substr(1);
  }
  
  string last = fullpath.substr(res+1);
  int lastindex;
  try {
    lastindex = atoi(last.c_str());
  }
  catch (exception &e) {
    BOOST_LOG_TRIVIAL(warning) << e.what();
    return;
  }
  
  // find the first part in the object.
  error_code ec;
  json *node = obj->find_pointer(path, ec);
  if (ec) {
    BOOST_LOG_TRIVIAL(error) << ec;
    return;
  }
  BOOST_LOG_TRIVIAL(trace) << path << ": " << *node;
  
  if (!(*node).is_array()) {
    BOOST_LOG_TRIVIAL(error) << "found node is not array";
    return;
  }
  
  // remove the element fron the array.
  boost::json::array newarr;
  auto arr = (*node).as_array();  
  for (int i=0; i<lastindex; i++) {
    newarr.push_back(arr[i]);
  }
  for (int i=lastindex+1; i<arr.size(); i++) {
    newarr.push_back(arr[i]);
  }
  
  // and then set the new array.
  obj->set_at_pointer(path, newarr);

}

void Security::addPolicy(json *obj, const string &type, const string &context, const string &id) {

  // get the index for the access
  int acc;
  if (type == "view") {
    acc = 0;
  }
  else if (type == "edit") {
    acc = 1;
  }
  else if (type == "exec") {
    acc = 2;
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "invalid type " << type;
    return;
  }
  auto accnode = obj->at("accesses").as_array()[acc];
//    BOOST_LOG_TRIVIAL(trace) << "access node " << accnode;
  
  // get the arrar
  boost::json::array arr;
  if (context == "group") {
    arr = accnode.at("groups").as_array();
  }
  else if (context == "user") {
    arr = accnode.at("users").as_array();
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "invalid context " << context;
    return;
  }
//    BOOST_LOG_TRIVIAL(trace) << "array " << arr;

  // copy over the existing array (transforming to strings)
  vector<string> newvec;
  transform(arr.begin(), arr.end(), back_inserter(newvec), [](auto e){ return e.as_string().c_str(); });
  
  // push in the new value
  newvec.push_back(id);
  
  // make sure the vector is sorted.
  sort(newvec.begin(), newvec.end());
  
  // create a new array.
  boost::json::array newarr;
  transform(newvec.begin(), newvec.end(), back_inserter(newarr), 
    [](auto e){ return boost::json::value_from(e); });
//    BOOST_LOG_TRIVIAL(trace) << "new array " << newarr;
  
  // and set it.
  if (context == "group") {
    obj->at("accesses").as_array()[acc].at("groups").emplace_array() = newarr;
  }
  else if (context == "user") {
    obj->at("accesses").as_array()[acc].at("users").emplace_array() = newarr;
  }

}

json Security::policyToQuery(const json &obj) {

  return { 
    { "accesses.0.name", "view" },
    { "accesses.0.groups", obj.at("accesses").as_array()[0].at("groups") }, 
    { "accesses.0.users", obj.at("accesses").as_array()[0].at("users") }, 
    { "accesses.1.name", "edit" },
    { "accesses.1.groups", obj.at("accesses").as_array()[1].at("groups") }, 
    { "accesses.1.users", obj.at("accesses").as_array()[1].at("users") }, 
    { "accesses.2.name", "exec" },
    { "accesses.2.groups", obj.at("accesses").as_array()[2].at("groups") }, 
    { "accesses.2.users", obj.at("accesses").as_array()[2].at("users") }
	};
}

optional<string> Security::modifyPolicy(const string &id, const vector<addTupleType> &add, const vector<string> &remove) {

  auto policy = Policy().findById(id).value();
  if (!policy) {
    BOOST_LOG_TRIVIAL(error) << "existing policy not found";
    return nullopt;
  }

  auto obj = policy.value().j();
  obj.as_object().erase("id");
  
  // do removes first.
  for (auto i: remove) {
    removeAt(&obj, i);
  }
  for (auto i: add) {
    addPolicy(&obj, get<0>(i), get<1>(i), get<2>(i));    
  }
  
//  BOOST_LOG_TRIVIAL(trace) << "new policy" << obj;
  
  // search for this query.
  auto existpolicy = Policy().find(policyToQuery(obj)).value();
  if (!existpolicy) {
    auto result = Policy().insert(obj);
    if (result) {
      regenerate();
    }
    return result;
  }
	
  return existpolicy.value().id();
  
}
  
string expandVar(const string &name) {

  wordexp_t p;
  wordexp(name.c_str(), &p, 0);
  stringstream ss;
  char** w = p.we_wordv;
  for (size_t i=0; i<p.we_wordc;i++ ) ss << w[i];
  wordfree(&p);

  return ss.str();
}

string getHome() {

  // try $NODES_HOME
  string home = expandVar("$NODES_HOME");
  if (home.size() == 0) {
    // use $HOME (like in production)
    home = expandVar("$HOME") + "/nodes";
  }
  BOOST_LOG_TRIVIAL(trace) << "home=" << home;
  
  return home;
  
}

void Security::regenerate() {

  string home = getHome();
  Policy().aggregate(home + "/scripts/groupeditpermissions.json");
  Policy().aggregate(home + "/scripts/groupviewpermissions.json");
  Policy().aggregate(home + "/scripts/usereditpermissions.json");
  Policy().aggregate(home + "/scripts/userviewpermissions.json");
}

void Security::regenerateGroups() {

  string home = getHome();
  Group().aggregate(home + "/scripts/useringroups.json");
}

string getFutureTime(int expires) {

  int hours = expires * 60 * 60 * 1000;
  time_t ts = (std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) * 1000) + hours;
  time_t tnum = ts / 1000;
  int secs = ts - (tnum * 1000);

  tm tm = *gmtime(&tnum);
  stringstream ss;
  ss << put_time(&tm, "%FT%T.");
  ss << secs;
  ss << "+00:00";

//  BOOST_LOG_TRIVIAL(trace) << ss.str();
  
  return ss.str();
  
}

optional<string> Security::generateShareLink(const string &me, const string &hostname, const string &streamid, const string &groupid, int expires) {

  auto stream = Stream().findById(streamid).value();
  if (!stream) {
    BOOST_LOG_TRIVIAL(error) << "Could not find stream.";
    return nullopt;
  }
  
	auto url = hostname + "/apps/chat/#/streams/" + stream.value().id();
  if (stream.value().streambits() && shareWithNewUsers) {
    auto token = createStreamShareToken(stream.value().id(), me, "mustName", groupid, getFutureTime(expires));
    if (token) {
      url += "?token=" + token.value();
    }
  }
  
  return url;
  
}

optional<string> Security::createStreamShareToken(const string &streamid, const string &me, const string &options, const string &groupid, const string &expires) {

  BOOST_LOG_TRIVIAL(trace) << "stream token will expire " << expires;
  BOOST_LOG_TRIVIAL(trace) << "now " << Date::toISODate(Date::now());

  json keyd = {
    { "id", streamid },
		{ "user", me },
		{ "options", options },
		{ "team", groupid },
		{ "expires", expires }
	};
  stringstream ss;
  ss << keyd;
  string data = ss.str();
  Encrypter encrypter(_key, _iv);
  return encrypter.encryptText(data);
  
}

optional<json> Security::expandStreamShareToken(const string &token) {

  Encrypter encrypter(_key, _iv);
  auto dec = encrypter.decryptText(token);
  if (dec) {
    return boost::json::parse(dec.value());
  }
  return nullopt;
  
}

string Security::newSalt() {

  unsigned char salt[SHA1_LEN];
  RAND_bytes(salt, sizeof(salt));
  return base64::to_base64(string((const char *)salt, sizeof(salt)));
  
}
    
string Security::newHash(const string &password, const string &salt) {

  unsigned char out[SHA1_LEN+1];
  int len = PKCS5_PBKDF2_HMAC_SHA1((const char *)password.c_str(), password.length(), (const unsigned char *)salt.c_str(), salt.length(), ITERATIONS, SHA1_LEN, out);
  return base64::to_base64(string((const char *)out, SHA1_LEN));

}

string Security::newPassword() {

  unsigned char password[24];
  RAND_bytes(password, sizeof(password));
  return boost::algorithm::hex(string((const char *)password, sizeof(password)));

}

