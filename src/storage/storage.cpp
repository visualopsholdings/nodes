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
#include "dict.hpp"

#include "log.hpp"
#include <fstream>
#include <wordexp.h>
#include <sstream>

using namespace vops;

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
    auto json = Dict::parseStream(file);
    if (!json) {
      L_ERROR("error loading schema from " << fn);
      return;
    }
    auto v = Dict::getVector(*json);
    if (!v) {
      L_ERROR("schema is not array");
      return;
    }
    // our schema contains objects rather than being a generic vector.
    _schema.clear();
    transform(v->begin(), v->end(), back_inserter(_schema), [](auto e) {
      auto o = Dict::getObject(e);
      if (o) {
        return *o;
      }
      return DictO();
    });
  }
  else {
    L_ERROR("schema file not found");
  }
  
}

optional<DictO> Storage::searchSchema(const string &type) {

  for (auto o: _schema) {
    auto t = Dict::getString(o, "type");
    if (t && *t == type) {
      return o;
    }
    auto subobj = Dict::getObject(o, "subobj");
    if (subobj) {
      t = Dict::getString(*subobj, "type");
      if (t && *t == type) {
        return *subobj;
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
    auto internal = Dict::getBool(scheme, "internal");
    if (internal && *internal) {
      L_TRACE(type << " is internal. Use the Schema objects");
      return false;
    }
  }
  
  auto coll = Dict::getString(scheme, "coll");
  
  *name = coll ? *coll: (type + "s");
  
  return true;

}

bool Storage::parentInfo(const string &type, string *parentfield, optional<string *> parenttype, optional<string *> namefield) {

  for (auto o: _schema) {
    auto subobj = Dict::getObject(o, "subobj");
    if (subobj) {
      auto subtype = Dict::getString(subobj.value(), "type");
      if (!subtype) {
        L_ERROR("type missing in schema subobj " << Dict::toString(subobj.value()));
        return false;
      }
      if (subtype.value() == type) {
        auto field = Dict::getString(subobj.value(), "field");
        if (!field) {
          L_ERROR("field missing in schema subobj " << Dict::toString(subobj.value()));
          return false;
        }
        *parentfield = field.value();
        if (parenttype) {
          auto type = Dict::getString(o, "type");
          if (type) {
            *(parenttype.value()) = type.value();
          }
        }
        if (namefield) {
          auto f = Dict::getString(subobj.value(), "namefield");
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

bool Storage::bulkInsert(const string &collName, const vector<DictO> &objs) {

  L_TRACE("bulkInsert to " << collName);
 
  auto schema = GenericSchema(collName);
  for (auto i: objs) {
  
    // the object we will be inserting.
    DictO obj;

    // get the id.
    string id;
    auto sid = Dict::getString(i, "_id");
    if (sid) {
      id = *sid;
    }
    else {
      auto oid = Dict::getString(i, "id");
      if (oid) {
        id = *oid;
        // no way to remove from an obj, just don't copy it.
      }
      else {
        L_ERROR("no id or _id");
        return false;
      }
    }
    
    // create the id.
    obj["_id"] = dictO({
      { "$oid", id }
    });

    // the modify date is not just a string.
    auto mod = Dict::getString(i, "modifyDate");
    if (mod) {
      L_TRACE("converting string modify date");
      obj["modifyDate"] = dictO({
        { "$date", Date::fromISODate(*mod) }
      });
    }
    
    // copy every other field.
    for (auto f: i) {
      auto key = get<0>(f);
      if (key != "_id" && key != "id" && key != "modifyDate") {
        obj[key] = get<1>(f);
      }
    }
    
    auto result = schema.insert(obj);
    if (!result) {
      L_ERROR("insert failed");
      return false;
    }
    if (result.value() == "exists") {
      DictO r;
      // copy all but "_id"
      for (auto f: obj) {
        auto key = get<0>(f);
        if (key != "_id") {
          r[key] = get<1>(f);
        }
      }
      auto result = schema.updateById(id, r);
      if (!result) {
        L_ERROR("update failed");
        return false;
      }
    }
  }
  return true;
  
}

DictO Storage::getNowO() {

  return dictO({ { "$date", Date::now() } });
  
}

Data Storage::getNow() {

  return { { "$date", Date::now() } };
  
}


bool Storage::hasArrayValue(const DictV &arr, const string &val) {

  return find_if(arr.begin(), arr.end(), [&](auto e) {
    auto s = Dict::getString(e);
    return s && *s == val;
  }) != arr.end();

}

bool Storage::appendArray(DictO *obj, const string &name, const string &val) {

  DictV arr;
  
  auto v = Dict::getVector(*obj, name);
  if (v) {
    copy(v->begin(), v->end(), back_inserter(arr));
  }
  
  if (hasArrayValue(arr, val)) {
    return false;
  }
  
  arr.push_back(val);
  
  (*obj)[name] = arr;
  
  return true;
  
}

bool Storage::arrayHas(const DictO &obj, const string &name, const string &val) {

  auto v = Dict::getVector(obj, name);
  if (!v) {
    return false;
  }
  return hasArrayValue(*v, val);

}

} // nodes