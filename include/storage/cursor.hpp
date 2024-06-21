/*
  cursor.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  DB Cursor for ZMQChat.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_cursor
#define H_cursor

#include <boost/json.hpp>
#include <memory>

using namespace std;
using json = boost::json::value;

class CursorImpl;

class Cursor {

public:
  
  optional<json> value();
    // return the value of the query as a JSON object.
    
  optional<json> values();
    // return the value of the query as a JSON array.

private:
  friend class Schema;
  
  Cursor(shared_ptr<CursorImpl> impl): _impl(impl) {};

  shared_ptr<CursorImpl> _impl;
  
};

#endif // H_cursor
