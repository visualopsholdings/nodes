/*
  dynamicrow.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 30-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage/dynamicrow.hpp"

#include "json.hpp"

#include <boost/log/trivial.hpp>

string DynamicRow::getString(const string &name) {

  auto value = Json::getString(_json, name);
  if (!value) {
    return "";
  }

  return value.value();

}

boost::json::array DynamicRow::getArray(const string &name) {

  auto value = Json::getArray(_json, name);
  if (!value) {
    return {};
  }

  return value.value();

}

vector<string> DynamicRow::getStringArray(const string &name) {

  vector<string> a;
  auto value = Json::getArray(_json, name);
  if (!value) {
    return a;
  }

  for (auto i: value.value()) {
    a.push_back(i.as_string().c_str());
  }

  return a;
  
}

bool DynamicRow::getBool(const string &name) {
  
  auto value = Json::getBool(_json, name);
  if (!value) {
    return false;
  }

  return value.value();

}


