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
using namespace nodes;

BOOST_AUTO_TEST_CASE( simple )
{
  cout << "=== simple ===" << endl;
  
//  cout << Encrypter::makeKey() << endl;
//  cout << Encrypter::makeIV() << endl;
  
  string test = "ABCDEFG";
  Encrypter encrypter(
    "90B21444AC1A2C9146FFA34C72BF787F76E7B0EDD236F0508571264153E58A787B82729268EF67DFCCC6B1F113721B0D752DA65DA6BC82BCA9A1C73E58DAAFF7", 
    "D764E7CAE16C361A4546873B");
  auto enc = encrypter.encryptText(test);
  BOOST_CHECK(enc);
  auto dec = encrypter.decryptText(enc.value());
  BOOST_CHECK(dec);
  BOOST_CHECK_EQUAL(dec.value(), test);
    
}
