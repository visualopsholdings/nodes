/*
  storagei.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef MONGO_DB

#include "storage/storagei.hpp"
#include "storage/collectioni.hpp"

using namespace nodes;

namespace nodes {

StorageImpl::StorageImpl() {
}

CollectionImpl StorageImpl::coll(const string &name) {

  return CollectionImpl(name);

}

} // nodes

#endif
