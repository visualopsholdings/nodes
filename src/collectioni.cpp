/*
  collectioni.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "collectioni.hpp"

#include "cursori.hpp"
#include "storage.hpp"

#include <mongocxx/cursor.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <sstream>

using bsoncxx::builder::basic::make_document;

Cursor CollectionImpl::find(const json &query) {
  stringstream ss;
  ss << query;
  bsoncxx::document::view_or_value q = bsoncxx::from_json(ss.str());
  return Cursor(shared_ptr<CursorImpl>(new CursorImpl(_c, q)));
}

Cursor CollectionImpl::find() {
  return Cursor(shared_ptr<CursorImpl>(new CursorImpl(_c, make_document())));
}

