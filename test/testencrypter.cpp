/*
  testencrypter.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 13-Sep-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "encrypter.hpp"

#include <iostream>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( simple )
{
  cout << "=== simple ===" << endl;
  
  string test = "ABCDEFG";
  Encrypter encrypter;
  auto enc = encrypter.encryptText(test);
  BOOST_CHECK(enc);
  auto dec = encrypter.decryptText(enc.value());
  BOOST_CHECK(dec);
  BOOST_CHECK_EQUAL(dec.value(), test);
    
}
