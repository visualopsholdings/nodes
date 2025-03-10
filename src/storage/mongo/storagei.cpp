/*
  storagei.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifdef MONGO_DB

#include "storage.hpp"

#include "storage/storagei.hpp"
#include "storage/collectioni.hpp"
#include "storage/resulti.hpp"
#include "date.hpp"
#include "log.hpp"

using namespace nodes;

namespace nodes {

StorageImpl::StorageImpl(const string &dbConn, const string &dbName) {

  _instance.reset(new mongocxx::instance());
 
  mongocxx::uri uri(dbConn);
  _client.reset(new mongocxx::client(uri));
  _db = (*_client)[dbName];

}

CollectionImpl StorageImpl::coll(const string &name) {

  return CollectionImpl(_db[name]);

}

} // nodes

#endif