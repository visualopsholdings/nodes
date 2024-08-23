/*
  storage.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage.hpp"

#include "storage/storagei.hpp"
#include "storage/collectioni.hpp"
#include "storage/resulti.hpp"

#include <boost/log/trivial.hpp>

shared_ptr<Storage> Storage::_instance;

void Storage::init(const string &dbConn, const string &dbName) {

  if (_impl.get()) {
    BOOST_LOG_TRIVIAL(trace) << "ignoring extra Storage::init";
    return;
  }
  _impl.reset(new StorageImpl(dbConn, dbName));

}

StorageImpl::StorageImpl(const string &dbConn, const string &dbName) {

  _instance.reset(new mongocxx::instance());
 
  mongocxx::uri uri(dbConn);
  _client.reset(new mongocxx::client(uri));
  _db = (*_client)[dbName];

}

CollectionImpl StorageImpl::coll(const string &name) {

  return CollectionImpl(_db[name]);
  
}

class GenericSchema: public Schema<DynamicRow> {

public:
  GenericSchema(const string &name): _name(name) {}
  
  virtual string collName() { return _name; };

private:
  string _name;
  
};

bool Storage::bulkInsert(const string &collName, boost::json::array &objs) {

  auto schema = GenericSchema(collName);
  for (auto i: objs) {
    json obj = i;
    auto id = obj.as_object()["_id"].as_string();
    obj.as_object()["_id"] = {
      { "$oid", id }
    };
    auto result = schema.insert(obj);
    if (!result) {
      BOOST_LOG_TRIVIAL(error) << "insert failed";
      return false;
    }
    if (result.value() == "exists") {
      obj.as_object().erase("_id");
      auto result = schema.updateById(id.c_str(), obj);
      if (!result) {
        BOOST_LOG_TRIVIAL(error) << "update failed";
        return false;
      }
    }
  }
  return true;
  
}
