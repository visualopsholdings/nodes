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
using namespace vops;

bool SchemaImpl::testInit() {

  if (!Storage::instance()->_impl) {
    L_ERROR("storage needs init");
    return false;
  }
  
  return true;
}

optional<int> SchemaImpl::rawUpdate(const DictO &query, const DictO &doc) {

  return updateGeneral(collName(), query, doc);

}

optional<int> SchemaImpl::update(const DictO &query, const DictO &doc) {

  return rawUpdate(query, dictO({
    { "$set", doc }
  }));

}

optional<string> SchemaImpl::updateById(const string &id, const DictO &doc) {

  return rawUpdateById(id, dictO({
    { "$set", doc }
  }));

}


