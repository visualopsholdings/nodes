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
#include "date.hpp"
#include "data.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include "log.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/filesystem.hpp>
#include <base64.hpp>
#include <cstdlib>
#include <ctime>
#include <iomanip>

#define SHA1_LEN    128
#define ITERATIONS  12000

using namespace vops;

namespace nodes {

shared_ptr<Security> Security::_instance;

string getHome();

Security::Security() {

  auto infos = Info().find({{ "type", { { "$in", {"tokenKey", "tokenIV"}}} }}, {"type", "text"}).all();
  if (!infos) {
    L_WARNING("missing infos. Running without can work.");
    return;
  }
  auto key = Info::getInfo(infos.value(), "tokenKey");
  if (!key) {
    L_ERROR("missing key info");
    return;
  }
  _key = key.value();
  auto iv = Info::getInfo(infos.value(), "tokenIV");
  if (!iv) {
    L_ERROR("missing iv info");
    return;
  }
  _iv = key.value();
  L_TRACE("key " << _key.length());
  L_TRACE("iv " << _iv.length());
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

Result<DynamicRow> Security::withView(const string &collection, optional<string> me, const Data &query, const vector<string> &fields) {

  if (me) {
    if (!isValidId(me.value())) {
      L_ERROR("me is not a valid Id.");
      return shared_ptr<ResultImpl>(0);
    }
    GroupViewPermissions groupviews;
    UserViewPermissions userviews;
    return SchemaImpl::findGeneral(collection, withQuery(groupviews, userviews, me.value(), query), fields);
  }

  return  SchemaImpl::findGeneral(collection, query, fields);
  
}

Result<DynamicRow> Security::withView(const string &collection, optional<string> me, const DictO &query, const vector<string> &fields) {

  if (me) {
    if (!isValidId(me.value())) {
      L_ERROR("me is not a valid Id.");
      return shared_ptr<ResultImpl>(0);
    }
    GroupViewPermissions groupviews;
    UserViewPermissions userviews;
    return SchemaImpl::findGeneral(collection, withQuery(groupviews, userviews, me.value(), query), fields);
  }

  return  SchemaImpl::findGeneral(collection, query, fields);
  
}

Result<DynamicRow> Security::withEdit(const string &collection, optional<string> me, const Data &query, const vector<string> &fields) {

  if (me) {
    if (!isValidId(me.value())) {
      L_ERROR("me is not a valid Id.");
      return shared_ptr<ResultImpl>(0);
    }
    GroupEditPermissions groupedits;
    UserEditPermissions useredits;
    return SchemaImpl::findGeneral(collection, withQuery(groupedits, useredits, me.value(), query), fields);
  }

  return  SchemaImpl::findGeneral(collection, query, fields);
  
}

bool Security::canEdit(const string &collection, optional<string> me, const string &id) {

  if (me) {
    if (!isValidId(me.value())) {
      L_ERROR("me is not a valid Id.");
      return false;
    }
    GroupEditPermissions groupedits;
    UserEditPermissions useredits;
    auto result = SchemaImpl::findGeneral(collection, withQuery(groupedits, useredits, me.value(), Data{ { "_id", { { "$oid", id } } } }), {});
    if (!result) {
      L_ERROR("can't find in canEdit");
      return false;
    }
    return result->value() != nullopt;
  }
    
  return true;
  
}

void Security::getPolicyUsers(const string &id, vector<string> *users) {

  // This is so much simpler than the Visual Ops version which determines the role of
  // each user.
  
  auto policy = Policy().findById(id, { "accesses" }).one();
  if (!policy) {
    L_ERROR("policy missing");
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
    auto groups = Group().findByIds(grps, { "members" }).all();
    for (auto g: groups.value()) {
      for (auto u: g.members()) addTo(users, u.user());
    }
  }

}

void Security::getPolicyGroups(const string &id, vector<string> *groups) {

  auto policy = Policy().findById(id, { "accesses" }).one();
  if (!policy) {
    L_ERROR("policy missing");
    return;
  }

  // collect the groups.
  vector<string> grps;
  for (auto a: policy.value().accesses()) {
    for (auto g: a.groups()) addTo(groups, g);
  }
 
}

Data Security::createArray(const vector<string> &list) {

  boost::json::array a;
  for (auto i: list) {
    a.push_back(boost::json::string(i));
  }
  return a;

}

DictV Security::createArray2(const vector<string> &list) {

  DictV v;
  copy(list.begin(), list.end(), back_inserter(v));
  return v;

}

void Security::queryIndexes(Schema<IndexRow> &schema, const vector<string> &inids, vector<string> *ids) {

  Data q = { { "_id", {{ "$in", createArray(inids) }}}};
  
  auto indexes = schema.find(q).all();
  if (!indexes) {
    L_TRACE("indexes missing for " << schema.collName());
    return;
  }
  for (auto i: indexes.value()) {
    for (auto j: i.all()) {
      addTo(ids, j);
    }
  }
  

}

Data Security::withQuery(Schema<IndexRow> &gperm, Schema<IndexRow> &uperm, const string &userid, const Data &query) {

  // collect all the groups the user is in.
  UserInGroups useringroups;
  auto indexes = useringroups.find({{ "_id", userid }}, {"value"}).one();
  vector<string> glist;
  if (indexes) {
    glist = indexes.value().all();
  }
  
  // collect all the policies for those groips
  vector<string> plist;
  queryIndexes(gperm, glist, &plist);
  
  // add all the policies just for this user.
  queryIndexes(uperm, { userid }, &plist);

  Data q = { { "$and", { 
    query,
    { { "policy", { { "$in", createArray(plist) } } } }
  } } };
  L_TRACE(q);

  return q;
  
}

DictO Security::withQuery(Schema<IndexRow> &gperm, Schema<IndexRow> &uperm, const string &userid, const DictO &query) {

  using vops::DictV;
  
  // collect all the groups the user is in.
  UserInGroups useringroups;
  auto indexes = useringroups.find({{ "_id", userid }}, {"value"}).one();
  vector<string> glist;
  if (indexes) {
    glist = indexes.value().all();
  }
  
  // collect all the policies for those groips
  vector<string> plist;
  queryIndexes(gperm, glist, &plist);
  
  // add all the policies just for this user.
  queryIndexes(uperm, { userid }, &plist);

  DictO q;
  DictV a;
  a.push_back(query);
  DictO policy;
  DictO in;
  in["$in"] = createArray2(plist);
  policy["policy"] = in;
  a.push_back(policy);
  q["$and"] = a;
  L_TRACE("query " << Dict::toString(q));
  
//   Data q = { { "$and", { 
//     query,
//     { { "policy", { { "$in", createArray2(plist) } } } }
//   } } };
//   L_TRACE(q);

  return q;
  
}

Data Security::makeLine(const string &type, int access, const string &name, const vector<string> &ids, int index) {

  // Lines have this simple format so they can be easily edited in a visual editor.

  boost::json::object line;
  
  stringstream ss;
  ss << "//accesses/" << access << "/" << type << "s/" << index;
  line["path"] = ss.str();
  line["type"] = name;
  line["context"] = type;
  line["id"] = ids[index];
  L_TRACE(ids[index]);
  if (type == "user") {
    auto user = User().findById(ids[index], { "fullname" }).one();
    line["name"] = !user ? "???" : user.value().fullname();
  }
  else {
    auto group = Group().findById(ids[index], { "name" }).one();
    line["name"] = !group ? "???" : group.value().name();
  }
  return line;
  
}

optional<Data> Security::getPolicyLines(const string &id) {

  auto policy = Policy().findById(id, { "accesses" }).one();
  if (!policy) {
    L_ERROR("policy missing");
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
  
  return Data(lines);
  
}

optional<string> Security::findPolicyForUser(const string &userid) {
  
  boost::json::array empty;
  Data obj = {
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
    
  auto policy = Policy().find(policyToQuery(obj)).one();
  if (!policy) {
    auto newpolicy = Policy().insert(obj);
    if (newpolicy) {
      regenerate();
    }
    else {
      L_ERROR("could not create new policy");
    }
    return newpolicy;
  }
  
  return policy.value().id();
  
}

void removeAt(boost::json::value *obj, const string &fullpath) {

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
    L_WARNING("remove doesn't have correct syntax " << fullpath);
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
    L_WARNING(e.what());
    return;
  }
  
  // find the first part in the object.
  error_code ec;
  json *node = obj->find_pointer(path, ec);
  if (ec) {
    L_ERROR(ec);
    return;
  }
  L_TRACE(path << ": " << *node);
  
  if (!(*node).is_array()) {
    L_ERROR("found node is not array");
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

void addPolicy(boost::json::value *obj, const string &type, const string &context, const string &id) {

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
    L_ERROR("invalid type " << type);
    return;
  }
  auto accnode = obj->at("accesses").as_array()[acc];
//    L_TRACE("access node " << accnode);
  
  // get the array
  boost::json::array arr;
  if (context == "group") {
    arr = accnode.at("groups").as_array();
  }
  else if (context == "user") {
    arr = accnode.at("users").as_array();
  }
  else {
    L_ERROR("invalid context " << context);
    return;
  }
//    L_TRACE("array " << arr);

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
//    L_TRACE("new array " << newarr);
  
  // and set it.
  if (context == "group") {
    obj->at("accesses").as_array()[acc].at("groups").emplace_array() = newarr;
  }
  else if (context == "user") {
    obj->at("accesses").as_array()[acc].at("users").emplace_array() = newarr;
  }

}

Data Security::policyToQuery(const Data &obj) {

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

  auto policy = Policy().findById(id).one();
  if (!policy) {
    L_ERROR("existing policy not found");
    return nullopt;
  }

  auto obj = policy.value().d();
  obj.as_object().erase("id");
  
  // do removes first.
  for (auto i: remove) {
    removeAt(&obj, i);
  }
  for (auto i: add) {
    addPolicy(&obj, get<0>(i), get<1>(i), get<2>(i));    
  }
  
//  L_TRACE("new policy" << obj);
  
  // search for this query.
  auto existpolicy = Policy().find(policyToQuery(obj)).one();
  if (!existpolicy) {
    auto result = Policy().insert(obj);
    if (result) {
      regenerate();
    }
    return result;
  }
	
  return existpolicy.value().id();
  
}
  
void Security::regenerate() {

  L_DEBUG("Building security...");
    
  string home = getHome();
  Policy().aggregate(home + "/scripts/groupeditpermissions.json");
  Policy().aggregate(home + "/scripts/groupviewpermissions.json");
  Policy().aggregate(home + "/scripts/usereditpermissions.json");
  Policy().aggregate(home + "/scripts/userviewpermissions.json");
}

void Security::regenerateGroups() {

  L_DEBUG("Building groups...");

  string home = getHome();
  Group().aggregate(home + "/scripts/useringroups.json");
}

enum CollectionBits {
  
  none = 0,
  shareWithNewUsers = (1 << 11)

};

optional<string> Security::generateShareLink(const string &me, const string &urlprefix, 
  const string &coll, const string &objid, const string &groupid, int expires, const string &bitsfield) {

  auto result = SchemaImpl::findByIdGeneral(coll, objid, {});
  if (!result) {
    L_ERROR("Can't find in " << coll);
    return nullopt;
  }
  auto obj = result->value();
  if (!obj) {
    L_ERROR("Could not find in " << coll);
    return nullopt;
  }
  auto bits = Json::getNumber(obj.value(), bitsfield);
  if (!bits) {
    L_ERROR("Obj had no " + bitsfield);
    return nullopt;
  }
  
  auto url = urlprefix;
  if (bits.value() && shareWithNewUsers) {
    auto token = createShareToken(objid, me, "mustName", groupid, Date::getFutureTime(Date::now(), expires));
    if (token) {
      url += "?token=" + token.value();
    }
  }
  
  return url;
  
}

optional<string> Security::createShareToken(const string &collid, const string &me, const string &options, const string &groupid, const string &expires) {

  L_TRACE("share token will expire " << expires);
  L_TRACE("now " << Date::toISODate(Date::now()));

  json keyd = {
    { "id", collid },
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

optional<Data> Security::expandShareToken(const string &token) {

  Encrypter encrypter(_key, _iv);
  auto dec = encrypter.decryptText(token);
  if (dec) {
    auto val = boost::json::parse(dec.value());
    return Data(val);
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

bool Security::isValidId(const string &id) {
  return id.size() == 24;
}

} // nodes
