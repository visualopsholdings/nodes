/*
  testvid.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include "vid.hpp"

#include <boost/algorithm/hex.hpp>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( TokenVID )
{
  cout << "=== TokenVID ===" << endl;
  
  VID vid("Vk9bWnCcuZDrTSmNgF0B1xomFRoZHhgKMgKXxqvBWg==");
  vid.describe();
  BOOST_CHECK(vid.valid());
  BOOST_CHECK_EQUAL(vid.password(), "d71a26151a191e180a320297c6abc15a");
  BOOST_CHECK_EQUAL(vid.uuid(), "5b5a709cb990eb4d298d805d");

}

BOOST_AUTO_TEST_CASE( PasswordVID )
{
  cout << "=== PasswordVID ===" << endl;
  
  VID vid("Vk9bWnCcuZDrTSmNgF0AcGFzc3dvcmQ=");
  vid.describe();
  BOOST_CHECK(vid.valid());
  BOOST_CHECK_EQUAL(vid.password(), "password");
  BOOST_CHECK_EQUAL(vid.uuid(), "5b5a709cb990eb4d298d805d");

}

BOOST_AUTO_TEST_CASE( BadVID )
{
  cout << "=== BadVID ===" << endl;
  
  VID vid("tracy");
  BOOST_CHECK(!vid.valid());

}

