/*
  storage.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "storage.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <boost/log/trivial.hpp>
#include <sstream>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

class StorageImpl {

private:
  friend class Storage;
  
  StorageImpl();

  shared_ptr<mongocxx::v_noabi::instance> _instance;
  shared_ptr<mongocxx::client> _client;
  mongocxx::database _db;
  
};

class CollectionImpl {

public:
  CollectionImpl(mongocxx::collection c): _c(c) {};
  CollectionImpl() {};

private:
  friend class Collection;
  
  mongocxx::collection _c;
  
};

class CursorImpl {

public:
  CursorImpl(mongocxx::collection c, bsoncxx::document::view_or_value q): _c(c), _q(q) {};

private:
  friend class Cursor;
  mongocxx::collection _c;
  bsoncxx::document::view_or_value _q;
  
};

Storage::Storage() {
  
  _impl.reset(new StorageImpl());
  
}

StorageImpl::StorageImpl() {

  _instance.reset(new mongocxx::instance());
  mongocxx::uri uri("mongodb://127.0.0.1:27017");
  _client.reset(new mongocxx::client(uri));
  _db = (*_client)["dev"];

}

Collection Storage::coll(const string &name) {

  if (!_impl->_db.has_collection(name)) {
    BOOST_LOG_TRIVIAL(error) << "coll " << name << " not found";
    return Collection(shared_ptr<CollectionImpl>(new CollectionImpl()));
  }
  
  return Collection(shared_ptr<CollectionImpl>(new CollectionImpl(_impl->_db[name])));
}

Cursor Collection::find(const json &query) {
  stringstream ss;
  ss << query;
  bsoncxx::document::view_or_value q = bsoncxx::from_json(ss.str());
  return Cursor(shared_ptr<CursorImpl>(new CursorImpl(_impl->_c, q)));
}

Cursor Collection::find() {
  return Cursor(shared_ptr<CursorImpl>(new CursorImpl(_impl->_c, make_document())));
}

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

