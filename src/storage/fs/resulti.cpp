/*
  fs/resulti.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef MONGO_DB

#include "storage/resulti.hpp"

using namespace nodes;

optional<Data> ResultImpl::value() {
  
  if (_ids) {
    return _c.findByIds(_ids.value());
  }
  return _c.find(_q, _sort);
  
}

optional<Data> ResultImpl::all() {
  
  if (_ids) {
    return _c.findByIds(_ids.value());
  }
  return _c.findAll(_q, _sort);
  
}

#endif
