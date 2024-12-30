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


optional<string> Data::getString(const string &name, bool silent) const {

  auto obj = getMember(*this, name, silent);
  if (!obj.is_string()) {
    if (!silent) {
      L_ERROR("obj is not string " << *this << " " << name);
    }
    return nullopt;
  }
  return boost::json::value_to<string>(obj);

}

optional<long> Data::getNumber(const string &name, bool silent) const {

  auto obj = getMember(*this, name, silent);
  if (!obj.is_int64()) {
    if (!silent) {
      L_ERROR("obj is not number " << *this << " " << name);
    }
    return nullopt;
  }
  return boost::json::value_to<long>(obj);

}

optional<bool> Data::getBool(const string &name, bool silent) const {

  auto obj = getMember(*this, name, silent);
  if (!obj.is_bool()) {
    if (!silent) {
      L_ERROR("obj is not bool " << *this << " " << name);
    }
    return nullopt;
  }
  return boost::json::value_to<bool>(obj);

}

optional<Data> Data::getData(const string &name, bool silent) {

  auto obj = getMember(*this, name, silent);
  if (!obj.is_array() && !obj.is_object()) {
    if (!silent) {
      L_ERROR("obj is not array or object " << *this << " " << name);
    }
    return nullopt;
  }
  
  return Data(obj);
  
}

void Data::setString(const string &name, const string &val) {

  if (!is_object()) {
    L_ERROR("obj not an object for setString " << name << "=" << val);
    return;
  }
  
  as_object()[name] = val;
  
}

void Data::setBool(const string &name, bool val) {

  if (!is_object()) {
    L_ERROR("obj not an object for setBool " << name << "=" << val);
    return;
  }
  
  as_object()[name] = val;
  
}

void Data::setObj(const string &name, const Data &val) {

  if (!is_object()) {
    L_ERROR("obj not an object for setObj " << name << "=" << val);
    return;
  }
  
  as_object()[name] = val;
  
}

boost::json::array::iterator Data::begin() {

  if (!is_array()) {
    L_ERROR("obj an array for begin");
    return 0;
  }
  
  return as_array().begin();
}

boost::json::array::iterator Data::end() {

  if (!is_array()) {
    L_ERROR("obj an array for end");
    return 0;
  }
  
  return as_array().end();
}

boost::json::array::const_iterator Data::begin() const {

  if (!is_array()) {
    L_ERROR("obj not an array for begin");
    return 0;
  }
  
  return as_array().begin();
}

boost::json::array::const_iterator Data::end() const {

  if (!is_array()) {
    L_ERROR("obj not an array for end");
    return 0;
  }
  
  return as_array().end();
}

size_t Data::size() const {

  if (!is_array()) {
    return 0;
  }
  
  return as_array().size();
  
}

void Data::push_back(const Data &data) {

  as_array().push_back(data);
  
}

} // nodes