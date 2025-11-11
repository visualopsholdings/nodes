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
#include "dict.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace nodes;
using namespace vops;

void dbSetup() {

  Storage::instance()->init("mongodb://127.0.0.1:27017", "dev");

}

BOOST_AUTO_TEST_CASE( userInGroups )
{
  cout << "=== userInGroups ===" << endl;
  
  dbSetup();
  Group().deleteMany(dictO({}));
  BOOST_CHECK(Group().insert(dictO({
    { "name", "Team 1" },
    { "members", DictV{
      dictO({ { "user", "u1" } }),
      dictO({ { "user", "u2" } })
      }
    }
  })));
  
  Group().aggregate("../scripts/useringroups.json");
  
  auto doc = UserInGroups().find(dictO({{ "_id", "u2"}}), {"value"}).one();
  BOOST_CHECK(doc);
  auto s = Dict::getString(doc->dict(), "value");
  BOOST_CHECK(s);
  BOOST_CHECK_EQUAL(s->size(), 24);

}
