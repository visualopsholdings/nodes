/*
  mongo/resulti.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifdef MONGO_DB

#include "storage/result.hpp"

#include "storage/resulti.hpp"
#include "storage/schemai.hpp"
#include "log.hpp"
#include "date.hpp"

#include <bsoncxx/json.hpp>
#include <bsoncxx/validate.hpp>
#include <bsoncxx/document/view.hpp>
#include <mongocxx/options/find.hpp>
#include <rfl/bson.hpp>

#include <sstream>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
using bsoncxx::type;

using namespace nodes;
using namespace vops;

template<typename T>
bool ResultImpl::fixDoc(bsoncxx::builder::basic::document *builder, const std::string &key, const T &view) {

//  L_TRACE("fixDoc?");

  auto oid = view["$oid"];
  if (oid) {
    builder->append(kvp(key, bsoncxx::oid(oid.get_string().value)));
//    L_TRACE("fixed $oid");
    return true;
  }
  auto date = view["$date"];
  if (date) {
    auto ms = chrono::milliseconds(date.get_int64().value);
    builder->append(kvp(key, bsoncxx::types::b_date(ms)));
//    L_TRACE("fixed $date " << ms);
    return true;
  }
  
  return false;
}

template<typename T>
bool ResultImpl::fixStrings(bsoncxx::builder::basic::document *builder, const T &k, bool stringids) {

  if (k.type() == type::k_string) {
    if (k.key() == "modifyDate") {
      // handle string modify dates.
      auto date = k.get_string().value;
      auto t = Date::fromISODate(string(date));
      auto ms = chrono::milliseconds(t);
      builder->append(kvp(k.key(), bsoncxx::types::b_date(ms)));
      return true;
    }
    if (k.key() == "_id" && !stringids) {
      auto id = k.get_string().value;
      // _id is used for oter things! so if it's not 24 chars long, then it's 
      // probs something else/
      if (id.size() == 24) {
//        L_TRACE("string _id " << id);
        builder->append(kvp("_id", bsoncxx::oid(id)));
        return true;
      }
    }
  }

  return false;
  
}

bsoncxx::document::view_or_value ResultImpl::convert(const DictO &obj, bool stringids) {

//  L_TRACE("convert " << Dict::toString(obj));
  
  // convert our obj directly to bson.
  auto bytes = rfl::bson::write<DictG>(obj);

  // but its not quite right. Convert the $oid into real ObjectIDs.
  // fix string dates and string _ids into correct things.
  auto view = bsoncxx::document::view((const std::uint8_t *)bytes.data(), bytes.size());
  auto doc = bsoncxx::builder::basic::document{};
  for (auto i: view) {
  
    if (i.type() == type::k_document) {
      auto view = i.get_document().view();
      if (i.key() == "$set") {
        L_TRACE("a $set document");
        auto set = bsoncxx::builder::basic::document{};
        for (auto k: view) {
          if (fixStrings(&set, k, stringids)) {
            continue;
          }
          set.append(kvp(k.key(), k.get_value()));
        }
        doc.append(kvp("$set", set));
        continue;
      }
      if (fixDoc(&doc, string(i.key()), view)) {
        continue;
      }
    }
    else if (i.type() == type::k_array) {
//      L_TRACE("found array " << i.key());
      auto as = bsoncxx::builder::basic::array{};
      for (auto j: i.get_array().value) {
        if (j.type() == type::k_document) {
          auto ds = bsoncxx::builder::basic::document{};
          auto view = j.get_document().view();
          for (auto k: view) {
            if (fixDoc(&ds, string(k.key()), k)) {
              continue;
            }
            ds.append(kvp(k.key(), k.get_value()));
          }
          as.append(ds);
          continue;
        }
        as.append(j.get_value());
      }
      doc.append(kvp(i.key(), as));
      continue;
    }
    else if (fixStrings(&doc, i, stringids)) {
      continue;
    }
    
    // copy the element right in.
    doc.append(kvp(i.key(), i.get_value()));
  }
  
//  L_TRACE("converted " << bsoncxx::to_json(doc));
  
  // make a COPY of this since we are leaving bytes behind above.
  return doc.extract();

}

std::optional<DictO> ResultImpl::convert(const bsoncxx::document::view &view) {

//  L_TRACE("convert " << bsoncxx::to_json(view));

  // create a new fixed document.
  auto doc = bsoncxx::builder::basic::document{};
  
  // fix it. If you don't do this the parser below will just fail. Have to take into account all
  // objects we support.
  fixBSONDoc(&doc, view);

  auto v2 = doc.view();
  
  // convert it directly to a JSON doc.
  auto dict = rfl::bson::read<DictG>(v2.data(), v2.length());
  if (!dict) {
    return nullopt;
  }
  
//  L_TRACE("converted " << Dict::toString(*dict));
  
  return Dict::getObject(*dict);
  
}

void ResultImpl::fixBSONDoc(bsoncxx::builder::basic::document *builder, const bsoncxx::document::view &view) {

  for (auto i: view) {
  
    switch (i.type()) {
    
      case type::k_double:
        L_ERROR("double not handled");
        break;
        
      case type::k_string:
//      case type::k_utf8:
//      L_TRACE("string");
        builder->append(kvp(i.key(), i.get_string().value));
        break;

      case type::k_document:
        L_ERROR("document not handled");
        break;
        
      case type::k_array:
//      L_TRACE("array");
        {
          auto as = bsoncxx::builder::basic::array{};
          for (auto j: i.get_array().value) {
            switch (j.type()) {
              case type::k_document:
                {
                  auto ds = bsoncxx::builder::basic::document{};
                  // recursively fix the docs inside.
                  fixBSONDoc(&ds, j.get_document().view());
                  as.append(ds);
                }
                break;
                
              case type::k_string:
                as.append(j.get_string().value);
                break;
                
              default:
                L_ERROR("BSON type not handled in array " << bsoncxx::to_json(i.get_array().value));
            }
          }
          builder->append(kvp(i.key(), as));
        }
        break;

      case type::k_binary:
        L_ERROR("binary not handled");
        break;
        
      case type::k_undefined:
        L_ERROR("undefined not handled");
        break;
        
      case type::k_oid:
//      L_TRACE("oid");
        {
          string oid = i.get_oid().value.to_string();
          if (i.key() == "_id") {
            builder->append(kvp("id", oid));
            continue;
          }
          builder->append(kvp(i.key(), oid));
        }
        break;

      case type::k_bool:
//      L_TRACE("bool");
        builder->append(kvp(i.key(), i.get_bool().value));
        break;

      case type::k_date:
//      L_TRACE("date");
        builder->append(kvp(i.key(), Date::toISODate(i.get_date().value.count())));
        break;

      case type::k_null:
        L_ERROR("null not handled");
        break;
        
      case type::k_regex:
        L_ERROR("regex not handled");
        break;
        
      case type::k_dbpointer:
        L_ERROR("dbpointer not handled");
        break;
        
      case type::k_code:
        L_ERROR("code not handled");
        break;
        
      case type::k_symbol:
        L_ERROR("symbol not handled");
        break;
        
      case type::k_codewscope:
        L_ERROR("codewscope not handled");
        break;
        
      case type::k_int32:
//      L_TRACE("int32");
        builder->append(kvp(i.key(), i.get_int32().value));
        break;

      case type::k_timestamp:
        L_ERROR("timestamp not handled");
        break;
        
      case type::k_int64:
//      L_TRACE("int64");
        builder->append(kvp(i.key(), i.get_int64().value));
        break;

     case type::k_minkey:
        L_ERROR("minkey not handled");
        break;
        
     case type::k_maxkey:
        L_ERROR("maxkey not handled");
        break;
        
      default:
        L_ERROR("BSON type not handled " << bsoncxx::to_json(view));
        
    }
    
  }
}

mongocxx::cursor ResultImpl::find() {

  mongocxx::options::find opts{};
  if (_f.size() > 0) {
    DictO o;
    for (auto i: _f) {
      o[i] = 1;
    }
    opts.projection(convert(o));
  }
  if (_limit) {
    opts.limit(_limit.value());
  }
  if (_sort) {
    opts.sort(_sort.value());
  }
  
  return _mc.find(_q, opts);

}

optional<DictO> ResultImpl::value() {
  
  if (!_mc) {
    return {};
  }
  
  mongocxx::cursor cursor = find();
  if (cursor.begin() == cursor.end()) {
    L_TRACE("empty");
    return {};
  }
  auto first = cursor.begin();
  auto obj = convert(*first);
  if (!obj) {
    L_ERROR("could not parse element " <<  bsoncxx::to_json(*first));
    return {};
  }
  
  return _c.fixObjects(*obj);
}

optional<DictV> ResultImpl::all() {
  
  if (!_mc) {
    return {};
  }
  
  mongocxx::cursor cursor = find();
  if (cursor.begin() == cursor.end()) {
    L_TRACE("empty");
    return {};
  }
  DictV val;
  for (auto i: cursor) {
    auto obj = convert(i);
    if (!obj) {
      L_ERROR("could not parse element " <<  bsoncxx::to_json(i));
      continue;
    }
    val.push_back(_c.fixObjects(*obj));
  }
  return val;
  
}

#endif

