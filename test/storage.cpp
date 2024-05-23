/*
  storage.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include "storage.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( users )
{
  cout << "=== users ===" << endl;
  
  Storage storage;
  json doc = storage.getUser("tracy");
  string id;
  try {
    string id = boost::json::value_to<string>(doc.at("_id").at("$oid"));
    cout << id << endl;
  }
  catch (...) {
    BOOST_FAIL("Exception");
  }
  
}
