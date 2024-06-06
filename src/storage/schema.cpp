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

Cursor User::find(const json &query, const vector<string> &fields) {
  return _storage._impl->coll("users").find(query, fields);
}

Policy::Policy(Storage &storage): _storage(storage) {
}

Cursor Policy::find(const json &query, const vector<string> &fields) {
  return _storage._impl->coll("policies").find(query, fields);
}

Stream::Stream(Storage &storage): _storage(storage) {
}

Cursor Stream::find(const json &query, const vector<string> &fields) {
  return _storage._impl->coll("streams").find(query, fields);
}
