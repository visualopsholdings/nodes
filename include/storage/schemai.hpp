/*
  schemai.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  DB Schemas for Nodes.

  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_schemai
#define H_schemai

#include "result.hpp"
#include "dynamicrow.hpp"
#include "dict.hpp"

using namespace std;
using vops::DictO;

namespace nodes {

class SchemaImpl {

public:
 
  void deleteMany(const DictO &doc) {
    deleteManyGeneral(collName(), doc);
  }
    // delete all documents that match the query.
    
  bool deleteById(const string &id);
    // delete the document that has this id.
    
  optional<string> insert(const DictO &doc) {
    return insertGeneral(collName(), doc);
  }
    // insert a new document.
    
  optional<int> update(const DictO &query, const DictO &doc);
    // update an existing document (inserts $set around the doc).
    
  optional<int> rawUpdate(const DictO &query, const DictO &doc);
    // update an existing document.
    
  optional<string> updateById(const string &id, const DictO &doc);
    // update an existing document when you know the ID  (inserts $set around the doc).
    
  optional<string> rawUpdateById(const string &id, const DictO &doc) {
    return updateGeneralById(collName(), id, doc);
  }
    // update an existing document when you know the ID.
    
  void aggregate(const string &filename);
    // process an aggregatin pipeline in the file.

  virtual string collName() = 0;
  
  static shared_ptr<ResultImpl> findGeneral(const string &collection, const DictO &query, 
          const vector<string> &fields, optional<int> limit=nullopt, optional<DictO> sort=nullopt);
  static shared_ptr<ResultImpl> findByIdGeneral(const string &collection, const string &id, const vector<string> &fields);
  static shared_ptr<ResultImpl> findByIdsGeneral(const string &collection, const vector<string> &ids, const vector<string> &fields);
  static int countGeneral(const string &collection, const DictO &query);
  static optional<int> updateGeneral(const string &collection, const DictO &query, const DictO &doc);
  static optional<string> updateGeneralById(const string &collection, const string &id, const DictO &doc);
  static optional<string> insertGeneral(const string &collection, const DictO &doc);
  static void deleteManyGeneral(const string &collection, const DictO &doc);
  static bool existsGeneral(const string &collection, const string &id);

  shared_ptr<ResultImpl> findResult(const DictO &query, const vector<string> &fields) {
    return findGeneral(collName(), query, fields);
  }
  shared_ptr<ResultImpl> findByIdResult(const string &id, const vector<string> &fields) {
    return findByIdGeneral(collName(), id, fields);
  }
  shared_ptr<ResultImpl> findByIdsResult(const vector<string> &ids, const vector<string> &fields) {
    return findByIdsGeneral(collName(), ids, fields);
  }
  
#ifdef MONGO_DB
  static shared_ptr<ResultImpl> findGeneral(const string &collection, bsoncxx::document::view_or_value query, 
          const vector<string> &fields, optional<int> limit=nullopt, optional<DictO> sort=nullopt);
  static int countGeneral(const string &collection, bsoncxx::document::view_or_value query);
  static bsoncxx::document::view_or_value idRangeAfterDateQuery(const vector<string> &ids, const string &date);
  static bsoncxx::document::view_or_value stringFieldEqualAfterDateQuery(const string &field, const string &id, const string &date);
  static bsoncxx::document::view_or_value boolFieldEqualAfterDateQuery(const string &field, bool value, const string &date);
  static bsoncxx::document::view_or_value afterDateQuery(const string &date);
  static bsoncxx::document::view_or_value idRangeQuery(const vector<string> &ids);
#endif

private:
  static bool testInit();

};

} // nodes

#endif // H_schemai
