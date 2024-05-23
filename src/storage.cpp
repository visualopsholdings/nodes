/*
  storage.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "storage.hpp"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

Storage::Storage() {

  _instance.reset(new mongocxx::instance());
  mongocxx::uri uri("mongodb://127.0.0.1:27017");
  _client.reset(new mongocxx::client(uri));
  
}

Storage::~Storage() {
}

json Storage::getUser(const string &name) {

  auto db = (*_client)["dev"];
  auto collection = db["users"];
  auto cursor = collection.find(make_document(kvp("name", name)));
  string s = bsoncxx::to_json(*cursor.begin());
  return boost::json::parse(s);

}
