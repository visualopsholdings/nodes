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
#include "data.hpp"
#include "json.hpp"

#include <bsoncxx/oid.hpp>
#include <fstream>
#include <sstream>

using namespace nodes;

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

optional<string> CollectionImpl::insert_one(const Data &doc) {
  
  L_TRACE("insert_one " << doc);
  
  string newid;
  Data newdoc = doc;
  auto id = doc.getData("_id", true);
  if (id) {
    auto oid = id->getString("$oid", true);
    if (!oid) {
      L_ERROR("no _id.$oid");
      return nullopt;
    }
    newid = oid.value();
  }
  else {
    bsoncxx::oid oid = bsoncxx::oid();
    newid = oid.to_string();
    Data id = {
      { "$oid", newid }
    };
    newdoc.setObj("_id", id);
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

  // and write the object out.
  newdoc.pretty_print(file);

  return newid;
  
}

bool CollectionImpl::match(const Data &json, const Data &query) {

  if (query.size() == 0) {
    return true;
  }
  auto q = query.getObject();
  for (auto elem: q) {
    auto s = json.getString(elem.key());
    if (s) {
      if (s.value() == elem.value().as_string()) {
        return true;
      }
    }
    else {
      auto n = json.getNumber(elem.key());
      if (n) {
        if (n.value() == elem.value().as_uint64()) {
          return true;
        }
      }
    }
  }
  return false;
  
}

Data CollectionImpl::find(const Data &query, optional<const Data> &sort) {

  L_TRACE("find " << query);

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
    Data j(input);
    if (match(j, query)) {
      return fixObjects(j);
    }
    
  }
        
  return {{}};
  
}

Data CollectionImpl::findAll(const Data &query, optional<const Data> &sort) {

  L_TRACE("findAll");
  
  boost::json::array a;
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
    Data j(input);
    if (match(j, query)) {
      a.push_back(fixObjects(j));
    }
    
  }
        
  return a;

}

Data CollectionImpl::findByIds(const vector<string> &ids) {

  L_TRACE("findByIds " << ids.size());
  
  boost::json::array a;
  for (auto id: ids) {
    string fn = getCollectionFolder() + "/" + *ids.begin() + ".json";
    L_TRACE(fn);
  
    ifstream f(fn);
    if (!f) {
      L_ERROR("could not open file " <<fn);
      return {{}};
    }
    string input(istreambuf_iterator<char>(f), {});
    Data j(input);
    a.push_back(fixObjects(j));
  }
  
  if (a.size() == 1) {
    return Data(*a.begin());
  }
  
  return Data(a);
  
}

void CollectionImpl::delete_many(const Data &doc) {

  L_TRACE("delete_many " << doc);
  
  if (doc.size() == 0) {
    filesystem::remove_all(getCollectionFolder());
    return;
  }

  L_ERROR("can only delete everything");
}

#endif

