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
#include "json.hpp"

#include <boost/log/trivial.hpp>
#include <fstream>
#include <wordexp.h>
#include <sstream>

shared_ptr<Storage> Storage::_instance;

string getHome();

void Storage::init(const string &dbConn, const string &dbName) {

  if (_impl.get()) {
    BOOST_LOG_TRIVIAL(trace) << "ignoring extra Storage::init";
    return;
  }
  _impl.reset(new StorageImpl(dbConn, dbName));

  allCollectionsChanged();
  
  ifstream file(getHome() + "/scripts/schema.json");
  if (file) {
    string input(istreambuf_iterator<char>(file), {});
    auto json = boost::json::parse(input);
    if (!json.is_array()) {
      BOOST_LOG_TRIVIAL(error) << "file does not contain array";
    }
    _schema = json.as_array();
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "schema file not found";
  }
  
}

string Storage::collName(const string &type) {

  // work out how to pluralise.
  auto scheme = find_if(_schema.begin(), _schema.end(), [&type](auto e) {
    auto t = Json::getString(e, "type");
    return t && t.value() == type;
  });
  string collname;
  if (scheme == _schema.end()) {
    collname = type + "s";
  }
  else {
    auto coll = Json::getString(*scheme, "coll", true);
      collname = coll ? coll.value() : (type + "s");
  }
  
  return collname;
}

string Storage::collName(const string &type, optional<string> coll) {

  return coll ? coll.value() : (type + "s");
  
}

optional<string> Storage::parentField(const string &type) {

  for (auto o: _schema) {
    auto subobj = Json::getObject(o, "subobj", true);
    if (subobj) {
      auto subtype = Json::getString(subobj.value(), "type");
      if (!subtype) {
        BOOST_LOG_TRIVIAL(error) << "type missing in schema subobj " << subobj.value();
        return nullopt;
      }
      if (subtype.value() == type) {
        auto field = Json::getString(subobj.value(), "field");
        if (!field) {
          BOOST_LOG_TRIVIAL(error) << "field missing in schema subobj " << subobj.value();
          return nullopt;
        }
        return field.value();
      }
    }
  }
  
  return nullopt;
  
}

string expandVar(const string &name) {

  wordexp_t p;
  wordexp(name.c_str(), &p, 0);
  stringstream ss;
  char** w = p.we_wordv;
  for (size_t i=0; i<p.we_wordc;i++ ) ss << w[i];
  wordfree(&p);

  return ss.str();
}

string getHome() {

  // try $NODES_HOME
  string home = expandVar("$NODES_HOME");
  if (home.size() == 0) {
    // use $HOME (like in production)
    home = expandVar("$HOME") + "/nodes";
  }
  BOOST_LOG_TRIVIAL(trace) << "home=" << home;
  
  return home;
  
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
    
//    BOOST_LOG_TRIVIAL(trace) << "insert obj " << obj;

    string id;
    if (obj.as_object().if_contains("_id")) {
      id = obj.as_object()["_id"].as_string();
    }
    else if (obj.as_object().if_contains("id")) {
      id = obj.as_object()["id"].as_string();
      obj.as_object().erase("id");
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
