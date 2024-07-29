/*
  storage.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "storage.hpp"

#include "storage/storagei.hpp"
#include "storage/collectioni.hpp"
#include "storage/resulti.hpp"

#include <boost/log/trivial.hpp>

shared_ptr<Storage> Storage::_instance;

void Storage::init(const string &dbConn, const string &dbName) {

  if (_impl.get()) {
    BOOST_LOG_TRIVIAL(trace) << "ignoring extra Storage::init";
    return;
  }
  _impl.reset(new StorageImpl(dbConn, dbName));

}

StorageImpl::StorageImpl(const string &dbConn, const string &dbName) {

  _instance.reset(new mongocxx::instance());
 
  mongocxx::uri uri(dbConn);
  _client.reset(new mongocxx::client(uri));
  _db = (*_client)[dbName];

}

CollectionImpl StorageImpl::coll(const string &name) {

  if (!_db.has_collection(name)) {
    BOOST_LOG_TRIVIAL(error) << "coll " << name << " not found";
    return CollectionImpl();
  }
  
  return CollectionImpl(_db[name]);
}
