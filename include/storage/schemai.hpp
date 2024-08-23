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
    
  optional<string> insert(const json &doc);
    // insert a new document.
    
  optional<string> update(const json &query, const json &doc);
    // update an existing document (inserts $set around the doc).
    
  optional<string> rawUpdate(const json &query, const json &doc);
    // update an existing document.
    
  optional<string> updateById(const string &id, const json &doc);
    // update an existing document when you know the ID  (inserts $set around the doc).
    
  optional<string> rawUpdateById(const string &id, const json &doc);
    // update an existing document when you know the ID.
    
  void aggregate(const string &filename);
    // process an aggregatin pipeline in the file.

  virtual string collName() = 0;
  
  shared_ptr<ResultImpl> findResult(const json &query, const vector<string> &fields);
  shared_ptr<ResultImpl> findByIdResult(const string &id, const vector<string> &fields);
  shared_ptr<ResultImpl> findByIdsResult(const vector<string> &ids, const vector<string> &fields);
  
};

#endif // H_schemai
