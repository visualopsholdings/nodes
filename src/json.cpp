/*
  json.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Jul-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "json.hpp"

#include "date.hpp"
#include "log.hpp"

string Json::toISODate(json &date) {

  if (!date.is_object()) {
    return "not_object";
  }
  
  if (!date.as_object().if_contains("$date")) {
    return "bad_object";
  }
  
  return Date::toISODate(date.at("$date").as_int64());
  
}

json Json::getMember(const json &j, const string &name, bool silent) {

  if (!j.is_object()) {
    if (!silent) {
      L_ERROR("json is not object");
    }
    return {};
  }
  if (!j.as_object().if_contains(name)) {
    return {};
  }
  return j.at(name);
  
}

bool Json::has(const json &j, const string &name) {

  return j.is_object() && j.as_object().if_contains(name);

}

optional<string> Json::getString(const json &j, const string &name, bool silent) {

  auto obj = getMember(j, name, silent);
  if (!obj.is_string()) {
    if (!silent) {
      L_ERROR("obj is not string " << j << " " << name);
    }
    return {};
  }
  return boost::json::value_to<string>(obj);

}

optional<boost::json::array> Json::getArray(json &j, const string &name, bool silent) {

  auto obj = getMember(j, name, silent);
  if (!obj.is_array()) {
    if (!silent) {
      L_ERROR("obj is not array " << j << " " << name);
    }
    return {};
  }
  return obj.as_array();
  
}

optional<json> Json::getObject(const json &j, const string &name, bool silent) {

  auto obj = getMember(j, name, silent);
  if (!obj.is_object()) {
    if (!silent) {
      L_ERROR("obj is not object " << j << " " << name);
    }
    return {};
  }
  return obj;
  
}

optional<bool> Json::getBool(const json &j, const string &name, bool silent) {

  auto obj = getMember(j, name, silent);
  if (!obj.is_bool()) {
    if (!silent) {
      L_ERROR("obj is not bool " << j << " " << name);
    }
    return {};
  }
  return boost::json::value_to<bool>(obj);

}

optional<long> Json::getNumber(const json &j, const string &name, bool silent) {

  auto obj = getMember(j, name, silent);
  if (!obj.is_int64()) {
    if (!silent) {
      L_ERROR("obj is not number " << j << " " << name);
    }
    return {};
  }
  return boost::json::value_to<long>(obj);

}

bool Json::hasArrayMember(const boost::json::object &obj, const string &name) {

  return obj.if_contains(name) && obj.at(name).is_array();
  
}

bool Json::hasArrayValue(const boost::json::array &arr, const string &val) {

  return find_if(arr.begin(), arr.end(), [&](auto e) { return e.as_string() == val; }) != arr.end();

}

bool Json::appendArray(boost::json::object *obj, const string &name, const string &val) {

  boost::json::array arr;
  
  if (hasArrayMember(*obj, name)) {
    auto a = obj->at(name).as_array();
    copy(a.begin(), a.end(), back_inserter(arr));
  }
  
  if (hasArrayValue(arr, val)) {
    return false;
  }
  
  arr.push_back(val.c_str());
  
  (*obj)[name] = arr;
  
  return true;
  
}

bool Json::arrayHas(const boost::json::object &obj, const string &name, const string &val) {

  if (!hasArrayMember(obj, name)) {
    return false;
  }
  return hasArrayValue(obj.at(name).as_array(), val);

}

bool Json::arrayTail(const boost::json::object &obj, const string &name, string *val) {

  if (!hasArrayMember(obj, name)) {
    return false;
  }
  auto a = obj.at(name).as_array();
  int asize = a.size();
  if (asize == 0) {
    return false;
  }
  if (!a[asize - 1].is_string()) {
    return false;
  }
  *val = a[asize - 1].as_string().c_str();
  return true;
  
}


