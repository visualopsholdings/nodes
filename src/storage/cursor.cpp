/*
  cursor.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "storage/cursor.hpp"

#include "storage/cursori.hpp"

#include <boost/log/trivial.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/options/find.hpp>
#include <sstream>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

mongocxx::cursor CursorImpl::find() {

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
  
  return _c.find(_q, opts);

}

optional<json> Cursor::value() {
  
  if (!_impl->_c) {
    return {};
  }
  
  mongocxx::cursor cursor = _impl->find();
  if (cursor.begin() == cursor.end()) {
    BOOST_LOG_TRIVIAL(error) << "not found";
    return {};
  }
  boost::json::array val;
  int count = 0;
  for (auto i: cursor) {
    string s = bsoncxx::to_json(i);
    val.push_back(boost::json::parse(s));
    count++;
  }
  if (count == 1) {
    return val[0];
  }
  
  BOOST_LOG_TRIVIAL(error) << "more than 1 value";
  return {};
  
}

optional<json> Cursor::values() {
  
  if (!_impl->_c) {
    return {};
  }
  
  mongocxx::cursor cursor = _impl->find();
  if (cursor.begin() == cursor.end()) {
    BOOST_LOG_TRIVIAL(error) << "not found";
    return {};
  }
  boost::json::array val;
  for (auto i: cursor) {
    val.push_back(boost::json::parse(bsoncxx::to_json(i)));
  }
  return val;
  
}

