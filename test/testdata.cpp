/*
  testdata.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 27-Dec-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "data.hpp"

#include <boost/json.hpp>
#include <iostream>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace nodes;

BOOST_AUTO_TEST_CASE( stringValue )
{
  cout << "=== stringValue ===" << endl;

  Data d = {
    { "str", "a string" }
  };
  auto str = d.getString("str");
  BOOST_CHECK(str);
  BOOST_CHECK_EQUAL(str.value(), "a string");
  
}

BOOST_AUTO_TEST_CASE( numValue )
{
  cout << "=== numValue ===" << endl;

  Data d = {
    { "num", 42 }
  };
  auto num = d.getNumber("num");
  BOOST_CHECK(num);
  BOOST_CHECK_EQUAL(num.value(), 42);
  
}

BOOST_AUTO_TEST_CASE( setString )
{
  cout << "=== setString ===" << endl;

  Data d = {
    { "num", 42 }
  };
  d.setString("parent", "a string");
  auto str = d.getString("parent");
  BOOST_CHECK(str);
  BOOST_CHECK_EQUAL(str.value(), "a string");
  
}

BOOST_AUTO_TEST_CASE( pop_back )
{
  cout << "=== pop_back ===" << endl;

  DataArray a;
  Data d(a);
  d.push_back({
    { "num", 1 }
  });
  d.push_back({
    { "num", 2 }
  });
  d.push_back({
    { "num", 3 }
  });
  BOOST_CHECK_EQUAL(d.size(), 3);
  Data n = d.getIterator(d.end()-1);
  BOOST_CHECK_EQUAL(n.getNumber("num").value(), 3);
  d.pop_back();
  BOOST_CHECK_EQUAL(d.size(), 2);
  n = d.getIterator(d.end()-1);
  BOOST_CHECK_EQUAL(n.getNumber("num").value(), 2);
  
}
