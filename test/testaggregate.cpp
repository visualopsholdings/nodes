/*
  testaggregate.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 26-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include <iostream>

#include "storage.hpp"
#include "storage/schemai.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace nodes;

void dbSetup() {

  Storage::instance()->init("mongodb://127.0.0.1:27017", "dev");

}

BOOST_AUTO_TEST_CASE( userInGroups )
{
  cout << "=== userInGroups ===" << endl;
  
  dbSetup();
  Group().deleteMany({{}});
  BOOST_CHECK(Group().insert({
    { "name", "Team 1" },
    { "members", {
      { { "user", "u1" } },
      { { "user", "u2" } }  
      } 
    }
  }));
  
  Group().aggregate("../scripts/useringroups.json");
  
  auto doc = UserInGroups().find({{ "_id", "u2"}}, {"value"}).one();
  BOOST_CHECK(doc);
  BOOST_CHECK(doc.value().d().is_object());
  BOOST_CHECK(doc.value().d().at("value").is_string());
  BOOST_CHECK_EQUAL(doc.value().d().at("value").as_string().size(), 24);

}
