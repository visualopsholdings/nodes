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

#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

class SchemaImpl {

public:
 
  void deleteMany(const json &doc);
    // delete all documents that match the query.
    
  bool deleteById(const string &id);
    // delete the document that has this id.
    
  optional<string> insert(const json &doc);
    // insert a new document.
    
  optional<string> update(const json &query, const json &doc);
    // update an existing document (inserts $set around the doc).
    
  optional<string> rawUpdate(const json &query, const json &doc);
    // update an existing document.
    
  optional<string> updateById(const string &id, const json &doc);
    // update an existing document when you know the ID  (inserts $set around the doc).
    
  optional<string> rawUpdateById(const string &id, const json &doc) {
    return updateGeneralById(collName(), id, doc);
  }
    // update an existing document when you know the ID.
    
  void aggregate(const string &filename);
    // process an aggregatin pipeline in the file.

  virtual string collName() = 0;
  
  static shared_ptr<ResultImpl> findGeneral(const string &collection, const json &query, const vector<string> &fields);
  static shared_ptr<ResultImpl> findGeneral(const string &collection, bsoncxx::document::view_or_value query, const vector<string> &fields);
  static shared_ptr<ResultImpl> findByIdGeneral(const string &collection, const string &id, const vector<string> &fields);
  static shared_ptr<ResultImpl> findByIdsGeneral(const string &collection, const vector<string> &ids, const vector<string> &fields);
  static int countGeneral(const string &collection, const json &query);
  static optional<string> updateGeneralById(const string &collection, const string &id, const json &doc);
    
  shared_ptr<ResultImpl> findResult(const json &query, const vector<string> &fields) {
    return findGeneral(collName(), query, fields);
  }
  shared_ptr<ResultImpl> findByIdResult(const string &id, const vector<string> &fields) {
    return findByIdGeneral(collName(), id, fields);
  }
  shared_ptr<ResultImpl> findByIdsResult(const vector<string> &ids, const vector<string> &fields) {
    return findByIdsGeneral(collName(), ids, fields);
  }
  
  static bsoncxx::document::view_or_value idRangeAfterDateQuery(const boost::json::array &ids, const string &date);
  static bsoncxx::document::view_or_value stringFieldEqualAfterDateQuery(const string &field, const string &id, const string &date);
  static bsoncxx::document::view_or_value boolFieldEqualAfterDateQuery(const string &field, bool value, const string &date);
  static bsoncxx::document::view_or_value afterDateQuery(const string &date);
  static bsoncxx::document::view_or_value idRangeQuery(const vector<string> &ids);

private:
  static bool testInit();

};

#endif // H_schemai
