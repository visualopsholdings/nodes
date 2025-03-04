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
#include "data.hpp"
#include "json.hpp"

#include "log.hpp"
#include <fstream>
#include <wordexp.h>
#include <sstream>

namespace nodes {

shared_ptr<Storage> Storage::_instance;

string getHome();

void Storage::init(const string &dbConn, const string &dbName, const string &schema) {

  if (_impl.get()) {
    L_TRACE("ignoring extra Storage::init");
    return;
  }
#ifdef MONGO_DB
  _impl.reset(new StorageImpl(dbConn, dbName));
#else
  _impl.reset(new StorageImpl());
#endif

  allCollectionsChanged();
  
  // use the schema name passed in
  string fn = schema.empty() ? getHome() + "/scripts/schema.json" : schema;
  
  ifstream file(fn);
  if (file) {
    string input(istreambuf_iterator<char>(file), {});
    try {
      auto json = boost::json::parse(input);
      if (!json.is_array()) {
        L_ERROR("file does not contain array");
      }
      _schema = json.as_array();
    }
    catch (exception &e) {
      L_ERROR("error loading schema " << e.what());
    }
  }
  else {
    L_ERROR("schema file not found");
  }
  
}

optional<Data> Storage::searchSchema(const string &type) {

  for (auto s: _schema) {
    auto t = Json::getString(s, "type");
    if (t && t.value() == type) {
      return Data(s.as_object());
    }
    auto subobj = Json::getObject(s, "subobj", true);
    if (subobj) {
      t = Json::getString(subobj.value(), "type");
      if (t && t.value() == type) {
        return Data(subobj.value().as_object());
      }
    }
  }
  return nullopt;
  
}

bool Storage::collName(const string &type, string *name, bool checkinternal) {

  auto scheme = searchSchema(type);
  if (!scheme) {
    L_TRACE(type << " is not in schema");
    return false;
  }
  
  if (checkinternal) {
    auto internal = Json::getBool(scheme.value(), "internal", true);
    if (internal && internal.value()) {
      L_TRACE(type << " is internal. Use the Schema objects");
      return false;
    }
  }
  
  auto coll = Json::getString(scheme.value(), "coll", true);
  
  *name = coll ? coll.value() : (type + "s");
  
  return true;

}

bool Storage::parentInfo(const string &type, string *parentfield, optional<string *> parenttype, optional<string *> namefield) {

  for (auto o: _schema) {
    auto subobj = Json::getObject(o, "subobj", true);
    if (subobj) {
      auto subtype = Json::getString(subobj.value(), "type");
      if (!subtype) {
        L_ERROR("type missing in schema subobj " << subobj.value());
        return false;
      }
      if (subtype.value() == type) {
        auto field = Json::getString(subobj.value(), "field");
        if (!field) {
          L_ERROR("field missing in schema subobj " << subobj.value());
          return false;
        }
        *parentfield = field.value();
        if (parenttype) {
          auto type = Json::getString(o, "type");
          if (type) {
            *(parenttype.value()) = type.value();
          }
        }
        if (namefield) {
          auto f = Json::getString(subobj.value(), "namefield", true);
          if (f) {
            *(namefield.value()) = f.value();
          }
          else {
            *(namefield.value()) = "name";
          }
        }
        return true;
      }
    }
  }
  
  return false;
  
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
  L_TRACE("home=" << home);
  
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
//  L_TRACE("collectionWasChanged " << name << " " << now);
  _changed[name] = now;
}

class GenericSchema: public Schema<DynamicRow> {

public:
  GenericSchema(const string &name): _name(name) {}
  
  virtual string collName() { return _name; };

private:
  string _name;
  
};

bool Storage::bulkInsert(const string &collName, Data &objs) {

  L_TRACE("bulkInsert to " << collName);
 
  auto schema = GenericSchema(collName);
  for (auto i: objs) {
    Data obj = i;
    
//    L_TRACE("insert obj " << obj);

    string id;
    if (obj.as_object().if_contains("_id")) {
      id = obj.as_object()["_id"].as_string();
    }
    else if (obj.as_object().if_contains("id")) {
      id = obj.as_object()["id"].as_string();
      obj.as_object().erase("id");
    }
    else {
      L_ERROR("no id or _id");
      return false;
    }
    
    obj.as_object()["_id"] = {
      { "$oid", id }
    };
    
    if (obj.as_object().if_contains("modifyDate") && obj.at("modifyDate").is_string()) {
      L_TRACE("converting string modify date");
      obj.as_object()["modifyDate"] = {
        { "$date", Date::fromISODate(obj.at("modifyDate").as_string().c_str()) }
      };
    }
    
    auto result = schema.insert(obj);
    if (!result) {
      L_ERROR("insert failed");
      return false;
    }
    if (result.value() == "exists") {
      obj.as_object().erase("_id");
      auto result = schema.updateById(id.c_str(), obj);
      if (!result) {
        L_ERROR("update failed");
        return false;
      }
    }
  }
  return true;
  
}

Data Storage::getNow() {

  return { { "$date", Date::now() } };
  
}

} // nodes