/*
  testb64.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 24-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "b64.hpp"

#include <iostream>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( simple )
{
  cout << "=== simple ===" << endl;
     
  string s = B64::toBinary("VGhpcyBpcyBhIHN0cmluZw==");

  BOOST_CHECK_EQUAL(s.substr(0, 16), "This is a string");
  
}
