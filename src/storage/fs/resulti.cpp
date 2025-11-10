/*
  fs/resulti.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef MONGO_DB

#include "storage/resulti.hpp"
#include "log.hpp"
#include "dict.hpp"

using namespace nodes;
using namespace vops;

optional<DictO> ResultImpl::value() {
  
  if (_ids) {
    auto v = _c.findByIds(_ids.value());
    if (v.size() > 1) {
      L_ERROR("more than 1 doc found!");
    }
    return Dict::getObject(v[0]);
  }
  return _c.find(_q, _sort);
  
}

optional<DictV> ResultImpl::all() {
  
  if (_ids) {
    return _c.findByIds(_ids.value());
  }
  return _c.findAll(_q, _sort);
  
}

#endif
