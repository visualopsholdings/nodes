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

optional<const Data> Data::getData(const string &name, bool silent) const {

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
    L_ERROR("obj not an array for begin");
    return 0;
  }
  
  return as_array().begin();
}

boost::json::array::iterator Data::end() {

  if (!is_array()) {
    L_ERROR("obj not an array for end");
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

  if (is_array()) {
    return as_array().size();
  }
  if (is_object()) {
    return as_object().size();
  }
  
  return 0;
  
}

void Data::push_back(const Data &data) {

  as_array().push_back(data);
  
}

Data Data::getIterator(boost::json::array::iterator i) {

  Data d(*i);
  return d;
  
}

void Data::pop_back() {
  
  as_array().pop_back();
  
}

boost::json::object Data::getObject() const {

  if (!this->is_object()) {
    L_ERROR("obj not object");
    return {{}};
  }
  return this->as_object();
}

void Data::pretty_print(ostream& os) const {

  pretty_print(os, *this, nullptr);
  
}

void Data::pretty_print(ostream& os, boost::json::value const& jv, string* indent) const {

  std::string indent_;
  if(! indent)
      indent = &indent_;
  switch(jv.kind())
  {
  case boost::json::kind::object:
  {
      os << "{\n";
      indent->append(4, ' ');
      auto const& obj = jv.get_object();
      if(! obj.empty())
      {
          auto it = obj.begin();
          for(;;)
          {
              os << *indent << boost::json::serialize(it->key()) << " : ";
              pretty_print(os, it->value(), indent);
              if(++it == obj.end())
                  break;
              os << ",\n";
          }
      }
      os << "\n";
      indent->resize(indent->size() - 4);
      os << *indent << "}";
      break;
  }

  case boost::json::kind::array:
  {
      os << "[\n";
      indent->append(4, ' ');
      auto const& arr = jv.get_array();
      if(! arr.empty())
      {
          auto it = arr.begin();
          for(;;)
          {
              os << *indent;
              pretty_print( os, *it, indent);
              if(++it == arr.end())
                  break;
              os << ",\n";
          }
      }
      os << "\n";
      indent->resize(indent->size() - 4);
      os << *indent << "]";
      break;
  }

  case boost::json::kind::string:
  {
      os << boost::json::serialize(jv.get_string());
      break;
  }

  case boost::json::kind::uint64:
  case boost::json::kind::int64:
  case boost::json::kind::double_:
      os << jv;
      break;

  case boost::json::kind::bool_:
      if(jv.get_bool())
          os << "true";
      else
          os << "false";
      break;

  case boost::json::kind::null:
      os << "null";
      break;
  }

  if(indent->empty())
      os << "\n";
}

} // nodes