/*
  testvid.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include "vid.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

BOOST_AUTO_TEST_CASE( PiVID )
{
  cout << "=== PiVID ===" << endl;
  
  VID vid("Vk9WNIdltNaXa0eOG9cAdmlzdWFsb3Bz");
  vid.describe();
  BOOST_CHECK(vid.valid());
  BOOST_CHECK_EQUAL(vid.password(), "visualops");
  BOOST_CHECK_EQUAL(vid.uuid(), "56348765B4D6976B478E1BD7");

}
