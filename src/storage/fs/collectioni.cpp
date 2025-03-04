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

#include <fstream>

using namespace nodes;

namespace nodes {
  string getHome();
}

string CollectionImpl::getCollectionFolder() {

  return getHome() + "/data/" + _name;

}

optional<string> CollectionImpl::insert_one(const Data &doc) {
  
  L_TRACE("insert_one " << doc);
  
  auto id = doc.getData("_id");
  if (!id) {
    L_ERROR("no _id");
    return nullopt;
  }
  
  auto oid = id->getString("$oid");
  if (!oid) {
    L_ERROR("no _id.$oid");
    return nullopt;
  }

  string fn = getCollectionFolder() + "/" + oid.value() + ".json";
  
  // and write the object out.
  ofstream file(fn);
  if (!file) {
    L_ERROR("couldn't open file " << fn);
    return nullopt;
  }

  // and write the object out.
  doc.pretty_print(file);

  return oid.value();
  
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
      return j;
    }
    
  }
        
  return {{}};
  
}

Data CollectionImpl::findAll(const Data &query, optional<const Data> &sort) {

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

#endif

