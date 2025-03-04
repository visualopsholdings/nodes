/*
  fs/schemai.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef MONGO_DB

#include "storage/schemai.hpp"

#include "storage.hpp"
#include "storage/storagei.hpp"
#include "log.hpp"

#include <fstream>

using namespace nodes;

int SchemaImpl::countGeneral(const string &collection, const Data &query) {

  if (!testInit()) {
    return 0;
  }

  return 0;

}

bool SchemaImpl::existsGeneral(const string &collection, const string &id) {

  if (!testInit()) {
    return 0;
  }

  return false;
  
}

shared_ptr<ResultImpl> SchemaImpl::findGeneral(const string &collection, const Data &query, 
          const vector<string> &fields, optional<int> limit, optional<Data> sort) {

  L_TRACE("find " << query << " in " << collection); 

  return shared_ptr<ResultImpl>(new ResultImpl(Storage::instance()->_impl->coll(collection), query, fields, limit));
  
}

shared_ptr<ResultImpl> SchemaImpl::findByIdGeneral(const string &collection, const string &id, const vector<string> &fields) {

  L_TRACE("find " << id << " in " << collection);

  return 0;
  
}

shared_ptr<ResultImpl> SchemaImpl::findByIdsGeneral(const string &collection, const vector<string> &ids, const vector<string> &fields) {

  L_TRACE("find ids " << ids.size() << " in " << collection);
  
  return 0;
  
}

void SchemaImpl::deleteManyGeneral(const string &collection, const Data &doc) {

  L_TRACE("deleteMany " << doc << " in " << collection);

  if (!testInit()) {
    return;
  }

  // TBD
  
}

bool SchemaImpl::deleteById(const string &id) {

  L_TRACE("deleteById " << id << " in " << collName());

  if (!testInit()) {
    return false;
  }
  
  return false;
    
}

optional<string> SchemaImpl::insertGeneral(const string &collection, const Data &doc) {

  L_TRACE("insert " << doc << " in " << collection);

  if (!testInit()) {
    return nullopt;
  }
  
  auto result = Storage::instance()->_impl->coll(collection).insert_one(doc);
  if (result) {
    Storage::instance()->collectionWasChanged(collection);
    return result.value();
  }

  return nullopt;

}

optional<int> SchemaImpl::updateGeneral(const string &collection, const Data &query, const Data &doc) {

  L_TRACE("update " << query << " in " << collection);

  if (!testInit()) {
    return nullopt;
  }
  
  return nullopt;

}

optional<string> SchemaImpl::updateGeneralById(const string &collection, const string &id, const Data &doc) {

  L_TRACE("update " << id << " in " << collection);

  if (!testInit()) {
    return nullopt;
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
  Data j(input);
  if (!j.is_array()) {
    L_ERROR("file does not contain array");
    return;    
  }
  
  // TBD

}

#endif

