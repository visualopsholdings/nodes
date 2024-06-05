/*
  collectioni.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  Collection implemementation for ZMQChat.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_collectioni
#define H_collectioni

#include <mongocxx/collection.hpp>
#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

class Cursor;

class CollectionImpl {

private:
  friend class User;
  friend class StorageImpl;
  
  CollectionImpl(mongocxx::collection c): _c(c) {};
  CollectionImpl() {};

  Cursor find(const json &query);
  Cursor find();

  mongocxx::collection _c;
  
};

#endif // H_collectioni
