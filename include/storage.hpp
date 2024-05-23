/*
  storage.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
    
  Storage for ZMQChat
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_storage
#define H_storage

#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

class Storage {

public:
  Storage();
  ~Storage();
  
  json getUser(const string &name);
  
private:
  shared_ptr<mongocxx::instance> _instance;
  shared_ptr<mongocxx::client> _client;
  
};

#endif // H_storage
