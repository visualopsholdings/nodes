/*
  result.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage/result.hpp"

#include "storage/resulti.hpp"
#include "storage/schemai.hpp"
#include "json.hpp"
#include "log.hpp"

#include <bsoncxx/json.hpp>
#include <mongocxx/options/find.hpp>
#include <sstream>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

using namespace nodes;

mongocxx::cursor ResultImpl::find() {

  mongocxx::options::find opts{};
  if (_f.size() > 0) {
    boost::json::object j;
    for (auto i: _f) {
      j[i] = 1;
    }
    stringstream ss;
    ss << j;
    opts.projection(bsoncxx::from_json(ss.str()));
  }
  if (_limit) {
    opts.limit(_limit.value());
  }
  if (_sort) {
    opts.sort(_sort.value());
  }
  
  return _c.find(_q, opts);

}

Data ResultImpl::fixObject(const Data &j) {

//  L_TRACE("fixing " << j);

  boost::json::object o;
  for (auto i: j.as_object()) {
  
//    L_TRACE("fixing " << i.key());
    
    if (i.value().is_object()) {
      if (i.key() == "_id" && i.value().as_object().if_contains("$oid")) {
        o["id"] = i.value().at("$oid");
        continue;
      }
      if (i.value().as_object().if_contains("$date")) {
        o[i.key()] = Json::toISODate(i.value());
        continue;
      }
      // recurse into sub objects.
      o[i.key()] = fixObject(i.value());
      continue;
    }
    
    if (i.value().is_array()) {
      boost::json::array newarray;
      auto arr = i.value().as_array();
      // copy array, recursing into sub objects.
      transform(arr.begin(), arr.end(), back_inserter(newarray), [this](auto e) {
        if (e.is_object()) {
          return fixObject(e); 
        }
        return Data(e);
      });
      o[i.key()] = newarray;
      continue;
    }
    
    o[i.key()] = i.value();
  }
  return o;

}

Data ResultImpl::fixObjects(const Data &j) {

//  L_TRACE("fixObjects " << j);

  return fixObject(j);

}

optional<Data> ResultImpl::value() {
  
  if (!_c) {
    return {};
  }
  
  mongocxx::cursor cursor = find();
  if (cursor.begin() == cursor.end()) {
    L_TRACE("empty");
    return {};
  }
  auto first = cursor.begin();
  auto jsons = bsoncxx::to_json(*first);
//  L_TRACE("raw json " << jsons);
  auto json = boost::json::parse(jsons);
//  L_TRACE("parsed json " << json);
  
  return fixObjects(json);
  
}

optional<Data> ResultImpl::all() {
  
  if (!_c) {
    return {};
  }
  
  mongocxx::cursor cursor = find();
  if (cursor.begin() == cursor.end()) {
    L_TRACE("empty");
    return {};
  }
  boost::json::array val;
  for (auto i: cursor) {
    auto jsons = bsoncxx::to_json(i);
//    L_TRACE("raw json " << jsons);
    auto json = boost::json::parse(jsons);
//    L_TRACE("parsed json " << json);
    val.push_back(fixObjects(json));
  }
  return Data(val);
  
}

