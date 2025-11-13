/*
  mongo/schemai.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifdef MONGO_DB

#include "storage/schemai.hpp"

#include "storage.hpp"
#include "storage/storagei.hpp"
#include "storage/collectioni.hpp"
#include "storage/resulti.hpp"
#include "date.hpp"
#include "dict.hpp"

#include "log.hpp"
#include <sstream>

#include <bsoncxx/json.hpp>
#include <bsoncxx/oid.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <fstream>

using namespace bsoncxx::builder::basic;
using namespace nodes;
using namespace vops;

shared_ptr<ResultImpl> SchemaImpl::findGeneral(const string &collection, bsoncxx::document::view_or_value query, 
          const vector<string> &fields, optional<int> limit, optional<DictO> sort) {

  if (!testInit()) {
    return 0;
  }

  if (sort) {
    return shared_ptr<ResultImpl>(new ResultImpl(Storage::instance()->_impl->coll(collection)._c, query, fields, limit, 
      bsoncxx::from_json(Dict::toString(sort.value()))));
  }

  return shared_ptr<ResultImpl>(new ResultImpl(Storage::instance()->_impl->coll(collection)._c, query, fields, limit, nullopt));

}

int SchemaImpl::countGeneral(const string &collection, bsoncxx::document::view_or_value query) {

  if (!testInit()) {
    return 0;
  }

  return Storage::instance()->_impl->coll(collection)._c.count_documents(query);
  
}

int SchemaImpl::countGeneral(const string &collection, const DictO &query) {

  if (!testInit()) {
    return 0;
  }

  bsoncxx::document::view_or_value q = bsoncxx::from_json(Dict::toString(query));

  return countGeneral(collection, q);

}

bool SchemaImpl::existsGeneral(const string &collection, const string &id) {

  if (!testInit()) {
    return 0;
  }

  bsoncxx::document::view_or_value q = make_document(kvp("_id", bsoncxx::oid(id)));

  return Storage::instance()->_impl->coll(collection)._c.count_documents(q) == 1;
  
}

shared_ptr<ResultImpl> SchemaImpl::findGeneral(const string &collection, const DictO &query, 
          const vector<string> &fields, optional<int> limit, optional<DictO> sort) {

  auto s = Dict::toString(query);
  L_TRACE("find " << s << " in " << collection); 

  bsoncxx::document::view_or_value q = bsoncxx::from_json(s);
  
  return findGeneral(collection, q, fields, limit, sort);
  
}

shared_ptr<ResultImpl> SchemaImpl::findByIdGeneral(const string &collection, const string &id, const vector<string> &fields) {

  L_TRACE("find " << id << " in " << collection);

  bsoncxx::document::view_or_value q = make_document(kvp("_id", bsoncxx::oid(id)));
  return findGeneral(collection, q, fields);
  
}

shared_ptr<ResultImpl> SchemaImpl::findByIdsGeneral(const string &collection, const vector<string> &ids, const vector<string> &fields) {

  L_TRACE("find ids " << ids.size() << " in " << collection);
  
  auto array = bsoncxx::builder::basic::array{};
  for (auto id: ids) {
    array.append(bsoncxx::oid(id));
  }

  bsoncxx::document::view_or_value q = make_document(kvp("_id", make_document(kvp("$in", array))));
  return findGeneral(collection, q, fields);
  
}

void SchemaImpl::deleteManyGeneral(const string &collection, const DictO &doc) {

  auto s = Dict::toString(doc);
  L_TRACE("deleteMany " << s << " in " << collection);

  if (!testInit()) {
    return;
  }

  bsoncxx::document::view_or_value d = bsoncxx::from_json(s);

  Storage::instance()->_impl->coll(collection)._c.delete_many(d);
  Storage::instance()->collectionWasChanged(collection);

}

bool SchemaImpl::deleteById(const string &id) {

  L_TRACE("deleteById " << id << " in " << collName());

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

optional<string> SchemaImpl::insertGeneral(const string &collection, const DictO &doc) {

  auto s = Dict::toString(doc);
  L_TRACE("insert " << s << " in " << collection);

  if (!testInit()) {
    return nullopt;
  }
  
  bsoncxx::document::view_or_value d = bsoncxx::from_json(s);
  
  try {
    auto result = Storage::instance()->_impl->coll(collection)._c.insert_one(d);
    if (result) {
      Storage::instance()->collectionWasChanged(collection);
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

optional<int> SchemaImpl::updateGeneral(const string &collection, const DictO &query, const DictO &doc) {

  auto s = Dict::toString(query);
  L_TRACE("update " << s << " in " << collection);

  if (!testInit()) {
    return nullopt;
  }

  bsoncxx::document::view_or_value q = bsoncxx::from_json(s);

  s = Dict::toString(doc);
  bsoncxx::document::view_or_value d = bsoncxx::from_json(s);
  
  auto result = Storage::instance()->_impl->coll(collection)._c.update_many(q, d);
  if (result) {
   Storage::instance()->collectionWasChanged(collection);
    return result->modified_count();
  }
  
  return nullopt;

}

optional<string> SchemaImpl::updateGeneralById(const string &collection, const string &id, const DictO &doc) {

  L_TRACE("update " << id << " in " << collection);

  if (!testInit()) {
    return nullopt;
  }

  bsoncxx::document::view_or_value q = make_document(kvp("_id", bsoncxx::oid(id)));

  auto s = Dict::toString(doc);
  bsoncxx::document::view_or_value d = bsoncxx::from_json(s);
  
  auto result = Storage::instance()->_impl->coll(collection)._c.update_one(q, d);
  if (result) {
    Storage::instance()->collectionWasChanged(collection);
    return "1";
   }
  
  return nullopt;

}

void SchemaImpl::aggregate(const string &filename) {

  if (!testInit()) {
    return;
  }

  ifstream file(filename);
  if (!file) {
    L_ERROR("file not found");
    return;    
  }
  string input(istreambuf_iterator<char>(file), {});
  auto obj = Dict::parseStream(file);
  if (!obj) {
    L_ERROR("file invalid");
    return;    
  }
  auto v = Dict::getVector(*obj);
  if (!v) {
    L_ERROR("file contents not an array");
    return;    
  }
  
  // build pipleline from JSON.
  mongocxx::pipeline p;
  for (auto i: *v) {
    auto obj = Dict::getObject(i);
    if (!obj) {
      L_ERROR("not object");
      continue;
    }
    auto first = *(obj->begin());
    string key = get<0>(first);
    auto value = get<1>(first);
    if (key == "$unwind") {
      auto path = Dict::getStringG(value, "path");
      if (!path) {
        L_ERROR("$unwind missing path");
        continue;
      }
      p.unwind(*path);
    }
    else if (key == "$out") {
      auto str = Dict::getString(value);
      if (!str) {
        L_ERROR("$out not a string");
        continue;
      }
      p.out(*str);
    }
    else {
      auto obj = Dict::getObject(value);
      if (!obj) {
        L_ERROR("key is not object " << key);
        return;
      }
      stringstream ss;
      ss << Dict::toString(*obj);
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
        L_ERROR("ignoring " << key);
      }
    }
  }
   
  // run aggregation.               
  auto cursor = Storage::instance()->_impl->coll(collName())._c.aggregate(p, mongocxx::options::aggregate{});
  if (cursor.begin() != cursor.end()) {
    L_INFO("aggregation had output");
  }
}

bsoncxx::document::view_or_value SchemaImpl::idRangeAfterDateQuery(const vector<string> &ids, const string &date) {

  L_TRACE("idRangeAfterDateQuery");
  
  auto qs = bsoncxx::builder::basic::array{};
  
  if (ids.size() == 1 && *(ids.begin()) == "*") {
    // don't include in the query, it's everything.
  }
  else {
    auto oids = bsoncxx::builder::basic::array{};
    for (auto u: ids) {
      try {
        oids.append(bsoncxx::oid(u));
      }
      catch (bsoncxx::exception &exc) {
        L_ERROR("idRangeAfterDateQuery " << exc.what());
      }
    }
    qs.append(make_document(kvp("_id", make_document(kvp("$in", oids)))));
  }
  
  // make a query with modify date.
  auto t = Date::fromISODate(date);
//  L_TRACE(t);
  auto d = bsoncxx::types::b_date(chrono::milliseconds(t));
//  L_TRACE(d);
  qs.append(make_document(kvp("modifyDate", make_document(kvp("$gt", d)))));
  
  // make the overall query.
  return make_document(kvp("$and", qs));

}

bsoncxx::document::view_or_value SchemaImpl::idRangeQuery(const vector<string> &ids) {

  auto oids = bsoncxx::builder::basic::array{};
  for (auto u: ids) {
    try {
      oids.append(bsoncxx::oid(u));
    }
    catch (bsoncxx::exception &exc) {
      L_ERROR("idRangeQuery " << exc.what());
    }
  }
  
  return make_document(kvp("_id", make_document(kvp("$in", oids))));

}

bsoncxx::document::view_or_value SchemaImpl::stringFieldEqualAfterDateQuery(const string &field, const string &id, const string &date) {

  L_TRACE("stringFieldEqualAfterDateQuery " << field << " " << id << " " << date);
  
  auto qs = bsoncxx::builder::basic::array{};
  
  qs.append(make_document(kvp(field, id)));
  
  // make a query with modify date.
  auto t = Date::fromISODate(date);
  auto d = bsoncxx::types::b_date(chrono::milliseconds(t));
  L_TRACE("d " << d);
  qs.append(make_document(kvp("modifyDate", make_document(kvp("$gt", d)))));
  
  // make the overall query.
  return make_document(kvp("$and", qs));

}

bsoncxx::document::view_or_value SchemaImpl::boolFieldEqualAfterDateQuery(const string &field, bool value, const string &date) {

  auto qs = bsoncxx::builder::basic::array{};
  
  qs.append(make_document(kvp(field, value)));
  
  // make a query with modify date.
  auto t = Date::fromISODate(date);
  auto d = bsoncxx::types::b_date(chrono::milliseconds(t));
  qs.append(make_document(kvp("modifyDate", make_document(kvp("$gt", d)))));
  
  // make the overall query.
  return make_document(kvp("$and", qs));

}

bsoncxx::document::view_or_value SchemaImpl::afterDateQuery(const string &date) {

  // make a query with modify date.
  auto t = Date::fromISODate(date);
  auto d = bsoncxx::types::b_date(chrono::milliseconds(t));
  
  // make the overall query.
  return make_document(kvp("modifyDate", make_document(kvp("$gt", d))));

}

#endif

