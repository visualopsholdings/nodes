/*
  fs/collectioni.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef MONGO_DB

#include "storage/collectioni.hpp"

#include "log.hpp"
#include "dict.hpp"

#include <bsoncxx/oid.hpp>
#include <fstream>
#include <sstream>

using namespace nodes;
using namespace vops;

namespace nodes {
  string getHome();
}

string CollectionImpl::getCollectionFolder() {

  string folder = getHome() + "/data/" + _name;
  if (!filesystem::exists(folder)) {
    filesystem::create_directories(folder);
  }
  return folder;
}

optional<string> CollectionImpl::insert_one(const DictO &doc) {
  
  L_TRACE("insert_one " << Dict::toString(doc));
  
  string newid;
  DictO newdoc = doc;
  auto id = Dict::getObject(doc, "_id");
  if (id) {
    auto oid = Dict::getString(*id, "$oid");
    if (!oid) {
      L_ERROR("no _id.$oid");
      return nullopt;
    }
    newid = oid.value();
  }
  else {
    bsoncxx::oid oid = bsoncxx::oid();
    newid = oid.to_string();
    auto id = dictO({
      { "$oid", newid }
    });
    newdoc["_id"] = id;
  }

  string fn = getCollectionFolder() + "/" + newid + ".json";
  L_TRACE(fn);
  
  if (filesystem::exists(fn)) {
    return "exists";
  }

  // and write the object out.
  ofstream file(fn);
  if (!file) {
    L_ERROR("couldn't open file " << fn);
    return nullopt;
  }

  // and write the object out as JSON
  file << Dict::toString(newdoc, true, ".json");

  return newid;
  
}

bool CollectionImpl::match(const DictO &doc, const DictO &query) {

  if (query.size() == 0) {
    return true;
  }
  for (auto elem: query) {
    auto key = get<0>(elem);
    auto value = get<1>(elem);
    
    auto s = Dict::getString(doc, key);
    if (s) {
      auto vals = Dict::getString(value);
      if (vals && *s == *vals) {
        return true;
      }
    }
    else {
      auto n = Dict::getNum(doc, key);
      if (n) {
        auto valn = Dict::getNum(value);
        if (valn && *n == *valn) {
          return true;
        }
      }
    }
  }
  return false;
  
}

DictO CollectionImpl::find(const DictO &query, optional<const DictO> &sort) {

  L_TRACE("find " << Dict::toString(query));

  for (auto const& e : filesystem::directory_iterator{getCollectionFolder()}) {
  
    if (!e.is_regular_file() || e.path().extension() != ".json") {
      continue;
    }
    ifstream f(e.path());
    if (!f) {
      L_ERROR("could not open file " << e.path());
      continue;
    }
    
    string input(istreambuf_iterator<char>(f), {});
    auto doc = Dict::parseString(input, ".json");
    if (doc) {
      auto obj = Dict::getObject(*doc);
      if (!obj) {
        L_ERROR("file contents not object!");
        continue;
      }
      if (match(*obj, query)) {
        return fixObjects(*obj);
      }
    }
    
  }
        
  return DictO{};
  
}

DictV CollectionImpl::findAll(const DictO &query, optional<const DictO> &sort) {

  L_TRACE("findAll");
  
  DictV a;
  for (auto const& e : filesystem::directory_iterator{getCollectionFolder()}) {
  
    if (!e.is_regular_file() || e.path().extension() != ".json") {
      continue;
    }
    ifstream f(e.path());
    if (!f) {
      L_ERROR("could not open file " << e.path());
      continue;
    }
    
    string input(istreambuf_iterator<char>(f), {});
    auto doc = Dict::parseString(input, ".json");
    if (doc) {
      auto obj = Dict::getObject(*doc);
      if (!obj) {
        L_ERROR("file contents not object!");
        continue;
      }
      if (match(*obj, query)) {
        a.push_back(fixObjects(*obj));
      }
    }
    
  }
        
  return a;

}

DictV CollectionImpl::findByIds(const vector<string> &ids) {

  L_TRACE("findByIds " << ids.size());
  
  DictV a;
  for (auto id: ids) {
    string fn = getCollectionFolder() + "/" + *ids.begin() + ".json";
    L_TRACE(fn);
  
    ifstream f(fn);
    if (!f) {
      L_ERROR("could not open file " <<fn);
      return {{}};
    }
    string input(istreambuf_iterator<char>(f), {});
    auto doc = Dict::parseString(input, ".json");
    if (doc) {
      auto obj = Dict::getObject(*doc);
      if (!obj) {
        L_ERROR("file contents not object!");
        continue;
      }
      a.push_back(fixObjects(*obj));
    }
  }
  
  return a;
  
}

void CollectionImpl::delete_many(const DictO &doc) {

  L_TRACE("delete_many " << Dict::toString(doc));
  
  if (doc.size() == 0) {
    filesystem::remove_all(getCollectionFolder());
    return;
  }

  L_ERROR("can only delete everything");
}

#endif

