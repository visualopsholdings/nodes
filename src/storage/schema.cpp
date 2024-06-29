/*
  schema.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "storage/schema.hpp"

#include "storage.hpp"
#include "storage/storagei.hpp"
#include "storage/collectioni.hpp"
#include "storage/cursori.hpp"

#include <boost/log/trivial.hpp>
#include <sstream>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <fstream>

using namespace bsoncxx::builder::basic;

Cursor Schema::find(const json &query, const vector<string> &fields) {

  BOOST_LOG_TRIVIAL(trace) << "find " << query;

  stringstream ss;
  ss << query;
  bsoncxx::document::view_or_value q = bsoncxx::from_json(ss.str());
  return Cursor(shared_ptr<CursorImpl>(new CursorImpl(Storage::instance()->_impl->coll(collName())._c, q, fields)));

}

Cursor Schema::findById(const string &id, const vector<string> &fields) {

  BOOST_LOG_TRIVIAL(trace) << "find " << id;

  bsoncxx::document::view_or_value q = make_document(kvp("_id", bsoncxx::oid(id)));
  return Cursor(shared_ptr<CursorImpl>(new CursorImpl(Storage::instance()->_impl->coll(collName())._c, q, fields)));
  
}

Cursor Schema::findByIds(const vector<string> &ids, const vector<string> &fields) {

  BOOST_LOG_TRIVIAL(trace) << "find ids " << ids.size();

  auto array = bsoncxx::builder::basic::array{};
  for (auto id: ids) {
    array.append(bsoncxx::oid(id));
  }
  bsoncxx::document::view_or_value q = make_document(kvp("_id", make_document(kvp("$in", array))));
  return Cursor(shared_ptr<CursorImpl>(new CursorImpl(Storage::instance()->_impl->coll(collName())._c, q, fields)));
  
}

void Schema::deleteMany(const json &doc) {

  BOOST_LOG_TRIVIAL(trace) << "deleteMany " << doc;

  stringstream ss;
  ss << doc;
  bsoncxx::document::view_or_value d = bsoncxx::from_json(ss.str());

  Storage::instance()->_impl->coll(collName())._c.delete_many(d);

}
  
optional<string> Schema::insert(const json &doc) {

  BOOST_LOG_TRIVIAL(trace) << "insert " << doc;

  stringstream ss;
  ss << doc;
  bsoncxx::document::view_or_value d = bsoncxx::from_json(ss.str());
  
  auto result = Storage::instance()->_impl->coll(collName())._c.insert_one(d);
  if (result) {
    return result.value().inserted_id().get_oid().value.to_string();
   }
  
  return nullopt;

}

void Schema::aggregate(const string &filename) {

  ifstream file(filename);
  if (!file) {
    BOOST_LOG_TRIVIAL(error) << "file not found";
    return;    
  }
  string input(istreambuf_iterator<char>(file), {});
  json j = boost::json::parse(input);
  if (!j.is_array()) {
    BOOST_LOG_TRIVIAL(error) << "file does not contain array";
    return;    
  }
  
  // build pipleline from JSON.
  mongocxx::pipeline p;
  for (auto i: j.as_array()) {
    auto first = i.as_object().begin();
    string key = first->key();
    auto value = first->value();
    if (key == "$unwind") {
      p.unwind(value.at("path").as_string().c_str());
    }
    else if (key == "$out") {
      p.out(value.as_string().c_str());
    }
    else {
      if (!value.is_object()) {
        BOOST_LOG_TRIVIAL(error) << "key is not object " << key;
        return;
      }
      stringstream ss;
      ss << value;
      bsoncxx::document::view_or_value d = bsoncxx::from_json(ss.str());
      if (key == "$group") {
        p.group(d);
      }
      else if (key == "$project") {
        p.project(d);
      }
      else if (key == "$match") {
        p.match(d);
      }
      else {
        BOOST_LOG_TRIVIAL(error) << "ignoring " << key;
      }
    }
  }
   
  // run aggregation.               
  auto cursor = Storage::instance()->_impl->coll(collName())._c.aggregate(p, mongocxx::options::aggregate{});
  if (cursor.begin() != cursor.end()) {
    BOOST_LOG_TRIVIAL(info) << "aggregation had output";
  }
}

