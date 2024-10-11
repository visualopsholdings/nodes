/*
  schema.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage/schemai.hpp"

#include "storage.hpp"
#include "storage/storagei.hpp"
#include "storage/collectioni.hpp"
#include "storage/resulti.hpp"
#include "date.hpp"

#include <boost/log/trivial.hpp>
#include <sstream>
#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <fstream>

using namespace bsoncxx::builder::basic;

shared_ptr<ResultImpl> SchemaImpl::findGeneral(const string &collection, bsoncxx::document::view_or_value query, const vector<string> &fields) {

  if (!testInit()) {
    return 0;
  }

  return shared_ptr<ResultImpl>(new ResultImpl(Storage::instance()->_impl->coll(collection)._c, query, fields));

}

int SchemaImpl::countGeneral(const string &collection, const json &query) {

  if (!testInit()) {
    return 0;
  }

  stringstream ss;
  ss << query;
  bsoncxx::document::view_or_value q = bsoncxx::from_json(ss.str());

  return Storage::instance()->_impl->coll(collection)._c.count_documents(q);
  
}

shared_ptr<ResultImpl> SchemaImpl::findGeneral(const string &collection, const json &query, const vector<string> &fields) {

  BOOST_LOG_TRIVIAL(trace) << "find " << query << " in " << collection; 

  stringstream ss;
  ss << query;
  bsoncxx::document::view_or_value q = bsoncxx::from_json(ss.str());
  return findGeneral(collection, q, fields);
  
}

shared_ptr<ResultImpl> SchemaImpl::findByIdGeneral(const string &collection, const string &id, const vector<string> &fields) {

  BOOST_LOG_TRIVIAL(trace) << "find " << id << " in " << collection;

  bsoncxx::document::view_or_value q = make_document(kvp("_id", bsoncxx::oid(id)));
  return findGeneral(collection, q, fields);
  
}

shared_ptr<ResultImpl> SchemaImpl::findByIdsGeneral(const string &collection, const vector<string> &ids, const vector<string> &fields) {

  BOOST_LOG_TRIVIAL(trace) << "find ids " << ids.size() << " in " << collection;
  
  auto array = bsoncxx::builder::basic::array{};
  for (auto id: ids) {
    array.append(bsoncxx::oid(id));
  }

  bsoncxx::document::view_or_value q = make_document(kvp("_id", make_document(kvp("$in", array))));
  return findGeneral(collection, q, fields);
  
}

void SchemaImpl::deleteMany(const json &doc) {

  BOOST_LOG_TRIVIAL(trace) << "deleteMany " << doc << " in " << collName();

  if (!testInit()) {
    return;
  }

  stringstream ss;
  ss << doc;
  bsoncxx::document::view_or_value d = bsoncxx::from_json(ss.str());

  Storage::instance()->_impl->coll(collName())._c.delete_many(d);
  Storage::instance()->collectionWasChanged(collName());
  
}
  
bool SchemaImpl::testInit() {

  if (!Storage::instance()->_impl) {
    BOOST_LOG_TRIVIAL(error) << "storage needs init";
    return false;
  }
  
  return true;
}

bool SchemaImpl::deleteById(const string &id) {

  BOOST_LOG_TRIVIAL(trace) << "deleteById " << id << " in " << collName();

  if (!testInit()) {
    return false;
  }
  
  bsoncxx::document::view_or_value q = make_document(kvp("_id", bsoncxx::oid(id)));

  auto result = Storage::instance()->_impl->coll(collName())._c.delete_one(q);
  if (result) {
    Storage::instance()->collectionWasChanged(collName());
    return result.value().deleted_count() == 1;
  }

  return false;
    
}
  
optional<string> SchemaImpl::insert(const json &doc) {

  BOOST_LOG_TRIVIAL(trace) << "insert " << doc << " in " << collName();

  if (!testInit()) {
    return nullopt;
  }
  
  stringstream ss;
  ss << doc;
  bsoncxx::document::view_or_value d = bsoncxx::from_json(ss.str());
  
  try {
    auto result = Storage::instance()->_impl->coll(collName())._c.insert_one(d);
    if (result) {
      Storage::instance()->collectionWasChanged(collName());
      return result.value().inserted_id().get_oid().value.to_string();
    }
  }
  catch (system_error &ex) {
    if (string(ex.code().category().name()) == "mongodb" && ex.code().value() == 11000) {
      return "exists";
    }
    throw ex;
  }
  
  return nullopt;

}

optional<string> SchemaImpl::rawUpdate(const json &query, const json &doc) {

  BOOST_LOG_TRIVIAL(trace) << "update " << query << " in " << collName();

  if (!testInit()) {
    return nullopt;
  }

  stringstream ss;
  ss << query;
  bsoncxx::document::view_or_value q = bsoncxx::from_json(ss.str());

  ss.str("");
  ss << doc;
  bsoncxx::document::view_or_value d = bsoncxx::from_json(ss.str());
  
  auto result = Storage::instance()->_impl->coll(collName())._c.update_one(q, d);
  if (result) {
    Storage::instance()->collectionWasChanged(collName());
    return "1";
  }
  
  return nullopt;

}

optional<string> SchemaImpl::update(const json &query, const json &doc) {

  return rawUpdate(query, {
    { "$set", doc }
  });

}

optional<string> SchemaImpl::rawUpdateById(const string &id, const json &doc) {

  BOOST_LOG_TRIVIAL(trace) << "update " << id << " in " << collName();

  if (!testInit()) {
    return nullopt;
  }

  bsoncxx::document::view_or_value q = make_document(kvp("_id", bsoncxx::oid(id)));

  stringstream ss;
  ss << doc;
  bsoncxx::document::view_or_value d = bsoncxx::from_json(ss.str());
  
  auto result = Storage::instance()->_impl->coll(collName())._c.update_one(q, d);
  if (result) {
    Storage::instance()->collectionWasChanged(collName());
    return "1";
   }
  
  return nullopt;

}

optional<string> SchemaImpl::updateById(const string &id, const json &doc) {

  return rawUpdateById(id, {
    { "$set", doc }
  });

}

void SchemaImpl::aggregate(const string &filename) {

  if (!testInit()) {
    return;
  }

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

bsoncxx::document::view_or_value SchemaImpl::idRangeAfterDateQuery(const boost::json::array &ids, const string &date) {

  auto qs = bsoncxx::builder::basic::array{};
  
  // convert all the user ids to oids.
  auto oids = bsoncxx::builder::basic::array{};
  for (auto u: ids) {
    oids.append(bsoncxx::oid(u.as_string().c_str()));
  }
  auto idrange = make_document(kvp("$in", oids));
  qs.append(make_document(kvp("_id", idrange)));
  
  // make a query with modify date.
  auto t = Date::fromISODate(date);
//  BOOST_LOG_TRIVIAL(trace) << t;
  auto d = bsoncxx::types::b_date(chrono::milliseconds(t));
//  BOOST_LOG_TRIVIAL(trace) << d;
  qs.append(make_document(kvp("modifyDate", make_document(kvp("$gt", d)))));
  
  // make the overall query.
  return make_document(kvp("$and", qs));

}

bsoncxx::document::view_or_value SchemaImpl::streamAfterDateQuery(const string &stream, const string &date) {

  auto qs = bsoncxx::builder::basic::array{};
  
  qs.append(make_document(kvp("stream", stream)));
  
  // make a query with modify date.
  auto t = Date::fromISODate(date);
  auto d = bsoncxx::types::b_date(chrono::milliseconds(t));
  qs.append(make_document(kvp("modifyDate", make_document(kvp("$gt", d)))));
  
  // make the overall query.
  return make_document(kvp("$and", qs));

}

bsoncxx::document::view_or_value SchemaImpl::upstreamAfterDateQuery(const string &date) {

  auto qs = bsoncxx::builder::basic::array{};
  
  qs.append(make_document(kvp("upstream", true)));
  
  // make a query with modify date.
  auto t = Date::fromISODate(date);
  auto d = bsoncxx::types::b_date(chrono::milliseconds(t));
  qs.append(make_document(kvp("modifyDate", make_document(kvp("$gt", d)))));
  
  // make the overall query.
  return make_document(kvp("$and", qs));

}

