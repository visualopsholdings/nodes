/*
  testdate.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Jul-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include <iostream>

#include "date.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using json = boost::json::value;

BOOST_AUTO_TEST_CASE( toISODate )
{
  cout << "=== toISODate ===" << endl;
  
  BOOST_CHECK_EQUAL(Date::toISODate(1721890479599), "2024-07-25T06:54:39.599+00:00");
  
}

BOOST_AUTO_TEST_CASE( fromISODate )
{
  cout << "=== fromISODate ===" << endl;
  
  BOOST_CHECK_EQUAL(Date::fromISODate("2024-07-25T06:54:39.599+00:00"), 1721890479599);
  
}

BOOST_AUTO_TEST_CASE( fromISODateShortMS )
{
  cout << "=== fromISODateShortMS ===" << endl;
  
  BOOST_CHECK_EQUAL(Date::fromISODate("2024-09-18T11:11:30.2+00:00"), 1726657890002);
  
}

BOOST_AUTO_TEST_CASE( inPast )
{
  cout << "=== inPast ===" << endl;
  
  BOOST_CHECK(Date::fromISODate("2024-07-01T06:54:39.599+00:00") < Date::now());
  BOOST_CHECK(Date::fromISODate("2027-07-01T06:54:39.599+00:00") > Date::now());
  
}

BOOST_AUTO_TEST_CASE( getFutureTimeMorning )
{
  cout << "=== getFutureTimeMorning ===" << endl;
  
  // 2024-07-25T06:54:39.599+00:00
  BOOST_CHECK_EQUAL(Date::getFutureTime(1721890479599, 4), "2024-07-25T10:54:39.599+00:00");
  
}

BOOST_AUTO_TEST_CASE( getFutureTimeAfterLunch )
{
  cout << "=== getFutureTimeAfterLunch ===" << endl;
  
  // 2024-09-18T11:11:30.2+00:00
  BOOST_CHECK_EQUAL(Date::getFutureTime(1726657890002, 4), "2024-09-18T15:11:30.2+00:00");
  
}

BOOST_AUTO_TEST_CASE( getFutureTimeEndOfMonth )
{
  cout << "=== getFutureTimeEndOfMonth ===" << endl;
  
  cout << Date::fromISODate("2024-09-30T22:11:30.2+00:00") << endl;
  
  // 2024-09-30T22:11:30.2+00:00
  BOOST_CHECK_EQUAL(Date::getFutureTime(1727734290002, 4), "2024-10-01T02:11:30.2+00:00");
  
}


