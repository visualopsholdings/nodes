/*
  data.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 27-Dec-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "data.hpp"

#include "log.hpp"

namespace nodes {

Data::Data(const string &s) {

  auto val = boost::json::parse(s);
  *this = val;
  
}

bool Data::has(const string &name) {

  return is_object() && as_object().if_contains(name);

}

boost::json::value getMember(const boost::json::value &j, const string &name, bool silent) {

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


optional<string> Data::getString(const string &name, bool silent) {

  auto obj = getMember(*this, name, silent);
  if (!obj.is_string()) {
    if (!silent) {
      L_ERROR("obj is not string " << *this << " " << name);
    }
    return nullopt;
  }
  return boost::json::value_to<string>(obj);

}

optional<long> Data::getNumber(const string &name, bool silent) {

  auto obj = getMember(*this, name, silent);
  if (!obj.is_int64()) {
    if (!silent) {
      L_ERROR("obj is not number " << *this << " " << name);
    }
    return nullopt;
  }
  return boost::json::value_to<long>(obj);

}

optional<bool> Data::getBool(const string &name, bool silent) {

  auto obj = getMember(*this, name, silent);
  if (!obj.is_bool()) {
    if (!silent) {
      L_ERROR("obj is not bool " << *this << " " << name);
    }
    return nullopt;
  }
  return boost::json::value_to<bool>(obj);

}

optional<Data> Data::getArray(const string &name, bool silent) {

  auto obj = getMember(*this, name, silent);
  if (!obj.is_array()) {
    if (!silent) {
      L_ERROR("obj is not array " << *this << " " << name);
    }
    return nullopt;
  }
  return Data(obj.as_array());
  
}

void Data::setString(const string &name, const string &val) {

  if (!is_object()) {
    L_ERROR("obj an object for setString " << name << "=" << val);
    return;
  }
  
  as_object()[name] = val;
  
}

void Data::setBool(const string &name, bool val) {

  if (!is_object()) {
    L_ERROR("obj an object for setBool " << name << "=" << val);
    return;
  }
  
  as_object()[name] = val;
  
}

void Data::setObj(const string &name, const Data &val) {

  if (!is_object()) {
    L_ERROR("obj an object for setObj " << name << "=" << val);
    return;
  }
  
  as_object()[name] = val;
  
}

} // nodes