/*
  testcursor.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 11-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include "storage/cursori.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( replaceIds )
{
  cout << "=== replaceIds ===" << endl;
  
  CursorImpl impl({}, {}, {});

  auto doc = impl.replaceIds({
    { "_id", {
        { "$oid", "xxxx" }
      }
    }
  });
  cout << doc << endl;
  BOOST_CHECK(doc.is_object());
  BOOST_CHECK(doc.at("id").is_string());
  BOOST_CHECK_EQUAL(doc.at("id").as_string(), "xxxx");
  
}

