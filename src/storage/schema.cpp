/*
  schema.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage/schema.hpp"

#include "dict.hpp"
#include "log.hpp"

#include <boost/algorithm/string.hpp>

using namespace vops;

namespace nodes {

vector<AccessRow> PolicyRow::accesses() { 

  vector<AccessRow> rows;
  auto accesses = Dict::getVector(dict(), "accesses");
  if (!accesses) {
    return rows;
  }
  transform(accesses->begin(), accesses->end(), back_inserter(rows), [](auto e) {
    auto obj = Dict::getObject(e);
    if (!obj) {
      L_ERROR("row in accesses not an object");
      return AccessRow(DictO());
    }
    return AccessRow(*obj);
  });
  return rows;
  
}

vector<MemberRow> GroupRow::members() { 

  vector<MemberRow> rows;
  auto members = Dict::getVector(dict(), "members");
  if (!members) {
    return rows;
  }
  transform(members->begin(), members->end(), back_inserter(rows), [](auto e) {
    auto obj = Dict::getObject(e);
    if (!obj) {
      L_ERROR("row in members not an object");
      return MemberRow(DictO());
    }
    return MemberRow(*obj);
  });
  return rows;
  
}

vector<string> IndexRow::all() {
  
  string value = getString("value");
  vector<string> values;
  boost::split(values, value, boost::is_any_of(","));
  return values;
  
}

optional<string> Info::getInfo(const vector<InfoRow> &infos, const string &type) {

  auto i = find_if(infos.begin(), infos.end(), 
    [&type](auto &e) { return e.type() == type; });
  if (i == infos.end()) {
    return nullopt;
  }
  return (*i).text();
}

string Info::getInfoSafe(optional<vector<InfoRow> > infos, const string &type, const string &def) {

  if (!infos) {
    return def;
  }
  auto v = getInfo(infos.value(), type);
  if (v) {
    return v.value();
  }
  return def;
  
}

} // nodes
