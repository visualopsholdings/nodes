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

Cursor Schema::find(const json &query, const vector<string> &fields) {

  stringstream ss;
  ss << query;
  bsoncxx::document::view_or_value q = bsoncxx::from_json(ss.str());
  return Cursor(shared_ptr<CursorImpl>(new CursorImpl(_storage._impl->coll(collName())._c, q, fields)));

}

void Schema::deleteMany(const json &doc) {

  BOOST_LOG_TRIVIAL(trace) << "deleteMany " << doc;

  stringstream ss;
  ss << doc;
  bsoncxx::document::view_or_value d = bsoncxx::from_json(ss.str());

  _storage._impl->coll(collName())._c.delete_many(d);

}
  
optional<string> Schema::insert(const json &doc) {

  stringstream ss;
  ss << doc;
  bsoncxx::document::view_or_value d = bsoncxx::from_json(ss.str());
  
  auto result = _storage._impl->coll(collName())._c.insert_one(d);
  if (result) {
    return result.value().inserted_id().get_oid().value.to_string();
   }
  
  return nullopt;

}
