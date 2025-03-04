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

using namespace nodes;

Data CollectionImpl::fixObject(const Data &j) {

//  L_TRACE("fixing " << j);

  boost::json::object o;
  for (auto i: j.as_object()) {
  
//    L_TRACE("fixing " << i.key());
    
    if (i.value().is_object()) {
      if (i.key() == "_id" && i.value().as_object().if_contains("$oid")) {
        o["id"] = i.value().at("$oid");
        continue;
      }
      if (i.value().as_object().if_contains("$date")) {
        o[i.key()] = Json::toISODate(i.value());
        continue;
      }
      // recurse into sub objects.
      o[i.key()] = fixObject(i.value());
      continue;
    }
    
    if (i.value().is_array()) {
      boost::json::array newarray;
      auto arr = i.value().as_array();
      // copy array, recursing into sub objects.
      transform(arr.begin(), arr.end(), back_inserter(newarray), [this](auto e) {
        if (e.is_object()) {
          return fixObject(e); 
        }
        return Data(e);
      });
      o[i.key()] = newarray;
      continue;
    }
    
    o[i.key()] = i.value();
  }
  return o;

}

Data CollectionImpl::fixObjects(const Data &j) {

//  L_TRACE("fixObjects " << j);

  return fixObject(j);

}

