/*
  mongo.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( users )
{
  cout << "=== users ===" << endl;
  
//   mongocxx::instance instance;
//   mongocxx::uri uri("mongodb://127.0.0.1:27017");
//   mongocxx::client client(uri);
//   auto db = client["dev"];
//   auto collection = db["users"];
//   auto cursor = collection.find({});
//   int count = 0;
//   for (auto doc : cursor) {
//     count++;
//   }
//   BOOST_CHECK_EQUAL(count, 1);
  
}
