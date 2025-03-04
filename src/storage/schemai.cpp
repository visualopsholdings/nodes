/*
  schemai.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage/schemai.hpp"

#include "storage.hpp"
#include "storage/storagei.hpp"
#include "storage/collectioni.hpp"
#include "storage/resulti.hpp"
#include "date.hpp"

#include "log.hpp"
#include <sstream>
#include <fstream>

using namespace nodes;

bool SchemaImpl::testInit() {

  if (!Storage::instance()->_impl) {
    L_ERROR("storage needs init");
    return false;
  }
  
  return true;
}

optional<int> SchemaImpl::rawUpdate(const Data &query, const Data &doc) {

  return updateGeneral(collName(), query, doc);

}

optional<int> SchemaImpl::update(const Data &query, const Data &doc) {

  return rawUpdate(query, {
    { "$set", doc }
  });

}

optional<string> SchemaImpl::updateById(const string &id, const Data &doc) {

  return rawUpdateById(id, {
    { "$set", doc }
  });

}


