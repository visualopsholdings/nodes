/*
  json.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include <boost/system/error_code.hpp>
#include <boost/json.hpp>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using json = boost::json::value;

BOOST_AUTO_TEST_CASE( goodString )
{
  cout << "=== goodString ===" << endl;
  
  json j = {
    { "str", "a string" }
  };
  try {
    boost::json::value_to<string>(j.at("str"));
  }
  catch (const boost::system::system_error& ex) {
    BOOST_FAIL("Threw");
    cout << ex.code().message() << endl;
  }
}

BOOST_AUTO_TEST_CASE( badString )
{
  cout << "=== badString ===" << endl;
  
  json j = {
    { "num", 1.1 }
  };
  try {
    boost::json::value_to<string>(j.at("num"));
    BOOST_FAIL("Didn't throw");
  }
  catch (const boost::system::system_error& ex) {
    cout << ex.code().message() << endl;
  }
}

BOOST_AUTO_TEST_CASE( missing )
{
  cout << "=== missing ===" << endl;
  
  json j = {
    { "xxx", 1.1 }
  };
  try {
    boost::json::value_to<string>(j.at("num"));
    BOOST_FAIL("Didn't throw");
  }
  catch (const boost::system::system_error& ex) {
    cout << ex.code().message() << endl;
  }
}

