/*
  collectioni.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage/collectioni.hpp"

#include "log.hpp"
#include "data.hpp"
#include "json.hpp"
#include "storage.hpp"
#include "date.hpp"

using namespace nodes;
using namespace vops;

DictO CollectionImpl::fixObject(const DictO &j) {

//  L_TRACE("fixing " << j);

  DictO o;
  for (auto i: j) {
  
    auto key = get<0>(i);
    L_TRACE("fixing " << key);
    
    auto val = get<1>(i);
    
    auto oval = Dict::getObject(val);
    if (oval) {
      L_TRACE("value is object");
      if (key == "_id") {
        L_TRACE("got _id");
        auto oid = Dict::getString(*oval, "$oid");
        if (oid) {
          L_TRACE("it's a string");
          o["id"] = *oid;
          continue;
        }
      }
      else if (oval->get("$date")) {
        o[key] = toISODate(*oval);
        continue;
      }
      o[key] = fixObject(*oval);
      continue;
    }
    
    auto iv = Dict::getVector(val);
    if (iv) {
      L_TRACE("value is vector");
      DictV newarray;
      // copy array, recursing into sub objects.
      transform(iv->begin(), iv->end(), back_inserter(newarray), [this](auto e) -> DictG {
        auto obj = Dict::getObject(e);
        if (obj) {
          return fixObject(*obj); 
        }
        return e;
      });
      o[key] = newarray;
      continue;
    }
    
    o[key] = val;
  }
  
  return o;

}

DictO CollectionImpl::fixObjects(const DictO &j) {

//  L_TRACE("fixObjects " << j);

  return fixObject(j);

}

string CollectionImpl::toISODate(const DictO &date) {
  
  auto d = Dict::getNum(date, "$date");
  if (!d) {
    return "bad_object";
  }
  
  return Date::toISODate(*d);
  
}


