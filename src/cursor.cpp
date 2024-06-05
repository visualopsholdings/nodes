/*
  cursor.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "cursor.hpp"

#include "cursori.hpp"

#include <boost/log/trivial.hpp>
#include <bsoncxx/json.hpp>

optional<json> Cursor::value() {
  
  if (!_impl->_c) {
    return {};
  }
  
  mongocxx::cursor cursor = _impl->_c.find(_impl->_q);
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
  
  mongocxx::cursor cursor = _impl->_c.find(_impl->_q);
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
  return val;
  
}

