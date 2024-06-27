/*
  testaggregate.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include "storage.hpp"
#include "storage/schema.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

// only one can be.
Storage storage("mongodb://127.0.0.1:27017", "dev");

BOOST_AUTO_TEST_CASE( userInGroups )
{
  cout << "=== userInGroups ===" << endl;
  
  Group(storage).deleteMany({{}});
  BOOST_CHECK(Group(storage).insert({
    { "name", "Team 1" },
    { "members", {
      { { "user", "u1" } },
      { { "user", "u2" } }  
      } 
    }
  }));
  
  Group(storage).aggregate("../src/useringroups.json");
  
  auto doc = UserInGroups(storage).find({{ "_id", "u2"}}, {"value"}).value();
  BOOST_CHECK(doc);
  BOOST_CHECK(doc.value().is_object());
  BOOST_CHECK(doc.value().at("value").is_string());
  BOOST_CHECK_EQUAL(doc.value().at("value").as_string().size(), 24);

}
