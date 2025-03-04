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
  
  auto json = _c.find(_q, _sort);  
  return _c.fixObjects(json);
  
}

optional<Data> ResultImpl::all() {
  
  return _c.findAll(_q, _sort);
  
}

#endif
