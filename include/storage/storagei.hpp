/*
  storagei.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  Storage implemementation for ZMQChat.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_storagei
#define H_storagei

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

using namespace std;

class CollectionImpl;

class StorageImpl {

public:  
  StorageImpl(const string &dbConn, const string &dbName);

  CollectionImpl coll(const string &name);

private:

  shared_ptr<mongocxx::v_noabi::instance> _instance;
  shared_ptr<mongocxx::client> _client;
  mongocxx::database _db;
  
};


#endif // H_storagei
