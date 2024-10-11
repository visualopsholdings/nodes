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
#include "date.hpp"

#include <boost/log/trivial.hpp>

shared_ptr<Storage> Storage::_instance;

void Storage::init(const string &dbConn, const string &dbName) {

  if (_impl.get()) {
    BOOST_LOG_TRIVIAL(trace) << "ignoring extra Storage::init";
    return;
  }
  _impl.reset(new StorageImpl(dbConn, dbName));

  allCollectionsChanged();
  
}

void Storage::allCollectionsChanged() {

  // touch all the collections.
  collectionWasChanged("users");
  collectionWasChanged("policies");
  collectionWasChanged("streams");
  collectionWasChanged("ideas");
  collectionWasChanged("groups");
  collectionWasChanged("infos");
  collectionWasChanged("sites");
  collectionWasChanged("nodes");

}

void Storage::collectionWasChanged(const string &name) {
  long now = Date::now();
//  BOOST_LOG_TRIVIAL(trace) << "collectionWasChanged " << name << " " << now;
  _changed[name] = now;
}

class GenericSchema: public Schema<DynamicRow> {

public:
  GenericSchema(const string &name): _name(name) {}
  
  virtual string collName() { return _name; };

private:
  string _name;
  
};

bool Storage::bulkInsert(const string &collName, boost::json::array &objs) {

  BOOST_LOG_TRIVIAL(trace) << "bulkInsert to " << collName;
 
  auto schema = GenericSchema(collName);
  for (auto i: objs) {
    json obj = i;
    
    string id;
    if (obj.as_object().if_contains("_id")) {
      id = obj.as_object()["_id"].as_string();
    }
    else if (obj.as_object().if_contains("id")) {
      id = obj.as_object()["id"].as_string();
    }
    else {
      BOOST_LOG_TRIVIAL(error) << "no id or _id";
      return false;
    }
    
    obj.as_object()["_id"] = {
      { "$oid", id }
    };
    
    if (obj.as_object().if_contains("modifyDate") && obj.at("modifyDate").is_string()) {
      BOOST_LOG_TRIVIAL(trace) << "converting string modify date";
      obj.as_object()["modifyDate"] = {
        { "$date", Date::fromISODate(obj.at("modifyDate").as_string().c_str()) }
      };
    }
    
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

json Storage::getNow() {

  return { { "$date", Date::now() } };
  
}
