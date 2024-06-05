/*
  schema.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "storage/schema.hpp"

#include "storage.hpp"
#include "storage/storagei.hpp"
#include "storage/collectioni.hpp"

User::User(Storage &storage): _storage(storage) {
}

Cursor User::find(const json &query) {
  return _storage._impl->coll("users").find(query);
}

Cursor User::find() {
  return _storage._impl->coll("users").find();
}
