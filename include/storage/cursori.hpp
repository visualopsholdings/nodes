/*
  cursori.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  DB Cursor implemementation for ZMQChat.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_cursori
#define H_cursori

#include <mongocxx/collection.hpp>
#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

class CursorImpl {

public:
  CursorImpl(mongocxx::collection coll, bsoncxx::document::view_or_value query, const vector<string> &fields): 
    _c(coll), _q(query), _f(fields) {};

  // public for testing.
  json replaceIds(const json &json);

private:
  friend class Cursor;
  
  mongocxx::cursor find();
  
  vector<string> _f;
  mongocxx::collection _c;
  bsoncxx::document::view_or_value _q;
  
};


#endif // H_cursori
