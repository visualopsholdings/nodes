/*
  schema.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  DB Schemas for ZMQChat.

  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_schema
#define H_schema

#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

class Storage;
class Cursor;

class Schema {
};

class User: public Schema {

public:
  User(Storage &storage);
  
  Cursor find(const json &query);
  Cursor find();

private:
  Storage &_storage;
  
};

#endif // H_schema
