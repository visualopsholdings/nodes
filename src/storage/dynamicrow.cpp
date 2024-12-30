/*
  dynamicrow.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 30-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage/dynamicrow.hpp"

#include "log.hpp"

using namespace nodes;

string DynamicRow::getString(const string &name) const {

  auto value = _data.getString(name);
  if (!value) {
    return "";
  }

  return value.value();

}

Data DynamicRow::getData(const string &name) {

  auto value = _data.getData(name);
  if (!value) {
    return {};
  }

  return value.value();

}

vector<string> DynamicRow::getStringArray(const string &name) {

  vector<string> a;
  auto value = _data.getData(name);
  if (!value) {
    return a;
  }

  for (auto i: value.value()) {
    a.push_back(i.as_string().c_str());
  }

  return a;
  
}

bool DynamicRow::getBool(const string &name, bool silent) const {
  
  auto value = _data.getBool(name, silent);
  if (!value) {
    return false;
  }

  return value.value();

}

int DynamicRow::getNumber(const string &name) const {
  
  auto value = _data.getNumber(name);
  if (!value) {
    return false;
  }

  return value.value();

}


