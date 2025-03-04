/*
  storagei.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-June-2024
    
  Storage implemementation for Nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_storagei
#define H_storagei

#ifdef MONGO_DB
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#endif
#include <string>

using namespace std;

namespace nodes {

class CollectionImpl;

class StorageImpl {

public:
#ifdef MONGO_DB
  StorageImpl(const string &dbConn, const string &dbName);
#else
  StorageImpl();
#endif
  CollectionImpl coll(const string &name);

private:

#ifdef MONGO_DB
  shared_ptr<mongocxx::v_noabi::instance> _instance;
  shared_ptr<mongocxx::client> _client;
  mongocxx::database _db;
#endif  
};

} // nodes

#endif // H_storagei
