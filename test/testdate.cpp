/*
  testdate.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include "json.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using json = boost::json::value;

BOOST_AUTO_TEST_CASE( utc )
{
  cout << "=== utc ===" << endl;
  
  json date = {
    { "$date", 1719816879599 }
  };

  BOOST_CHECK_EQUAL(Json::toISODate(date), "2024-07-01T06:54:39.599+00:00");
  
}

BOOST_AUTO_TEST_CASE( badDate )
{
  cout << "=== badDate ===" << endl;
  
  json date = {
    { "$xxx", 1719816879599 }
  };

  BOOST_CHECK_EQUAL(Json::toISODate(date), "bad_object");
  
}
