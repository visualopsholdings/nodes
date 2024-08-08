/*
  testserver.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 8-Aug-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include <iostream>

#include "server.hpp"
#include "storage.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( resetServer )
{
  cout << "=== resetServer ===" << endl;
  
  Server server(true, 0, 0, "mongodb://127.0.0.1:27017", "dev", "", "");
  BOOST_CHECK(server.resetServer());
  
}
