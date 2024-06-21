/*
  storage.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "storage.hpp"

#include "storage/storagei.hpp"
#include "storage/collectioni.hpp"
#include "storage/cursori.hpp"

#include <boost/log/trivial.hpp>

Storage::Storage() {
  
  _impl.reset(new StorageImpl());
  
}

StorageImpl::StorageImpl() {

  _instance.reset(new mongocxx::instance());
 
//  mongocxx::uri uri("mongodb://127.0.0.1:27017");
  mongocxx::uri uri("mongodb://fiveEstellas:visualops@localhost/fiveEstellas");
  _client.reset(new mongocxx::client(uri));
  _db = (*_client)["dev"];

}

CollectionImpl StorageImpl::coll(const string &name) {

  if (!_db.has_collection(name)) {
    BOOST_LOG_TRIVIAL(error) << "coll " << name << " not found";
    return CollectionImpl();
  }
  
  return CollectionImpl(_db[name]);
}
