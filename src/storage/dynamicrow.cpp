/*
  dynamicrow.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 30-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage/dynamicrow.hpp"

#include "log.hpp"
#include "dict.hpp"

using namespace nodes;
using namespace vops;

string DynamicRow::getString(const string &name) const {

  auto value = Dict::getString(_data, name);
  if (!value) {
    return "???";
  }

  return value.value();

}

DictO DynamicRow::getObject(const string &name) {

  auto value = Dict::getObject(_data, name);
  if (!value) {
    return {};
  }

  return value.value();

}

vector<string> DynamicRow::getStringArray(const string &name) {

  vector<string> a;
  auto value = Dict::getVector(_data, name);
  if (!value) {
    return a;
  }

  for (auto i: value.value()) {
    auto s = Dict::getString(i);
    if (s) {
      a.push_back(*s);
    }
    else {
      a.push_back("???");
    }
  }

  return a;
  
}

bool DynamicRow::getBool(const string &name, bool silent) const {
  
  auto value = Dict::getBool(_data, name);
  if (!value) {
    return false;
  }

  return value.value();

}

int DynamicRow::getNumber(const string &name) const {
  
  auto value = Dict::getNum(_data, name);
  if (!value) {
    return 0;
  }

  return value.value();

}
