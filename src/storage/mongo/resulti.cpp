/*
  mongo/resulti.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifdef MONGO_DB

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
  
  return _mc.find(_q, opts);

}

optional<Data> ResultImpl::value() {
  
  if (!_mc) {
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
  
  
  return _c.fixObjects(json);
}

optional<Data> ResultImpl::all() {
  
  if (!_mc) {
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
    val.push_back(_c.fixObjects(json));
  }
  return Data(val);
  
}

#endif

