/*
  testjson.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include <iostream>

#include "json.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using json = boost::json::value;
using namespace nodes;

BOOST_AUTO_TEST_CASE( goodString )
{
  cout << "=== goodString ===" << endl;
  
  json j = {
    { "str", "a string" }
  };
  auto str = Json::getString(j, "str");
  BOOST_CHECK(str);
  BOOST_CHECK_EQUAL(str.value(), "a string");
  
}

BOOST_AUTO_TEST_CASE( badString )
{
  cout << "=== badString ===" << endl;
  
  json j = {
    { "num", 1.1 }
  };
  auto num = Json::getString(j, "num");
  BOOST_CHECK(!num);

}

BOOST_AUTO_TEST_CASE( missing )
{
  cout << "=== missing ===" << endl;
  
  json j = {
    { "xxx", 1.1 }
  };
  auto num = Json::getString(j, "num");
  BOOST_CHECK(!num);

}

BOOST_AUTO_TEST_CASE( goodBool )
{
  cout << "=== goodBool ===" << endl;
  
  json j = {
    { "xxx", true }
  };
  auto xxx = Json::getBool(j, "xxx");
  BOOST_CHECK(xxx);

}

BOOST_AUTO_TEST_CASE( badBool )
{
  cout << "=== badBool ===" << endl;
  
  json j = {
    { "xxx", "yyyy" }
  };
  auto xxx = Json::getBool(j, "xxx");
  BOOST_CHECK(!xxx);

}

BOOST_AUTO_TEST_CASE( goodArray )
{
  cout << "=== goodArray ===" << endl;
  
  json j = {
    { "xxx", { {
      { "aaa", 42 }
    } } }
  };
  auto xxx = Json::getArray(j, "xxx");
  BOOST_CHECK(xxx);
  BOOST_CHECK_EQUAL(xxx.value().size(), 1);
  BOOST_CHECK(xxx.value()[0].is_object());
  BOOST_CHECK(xxx.value()[0].at("aaa").is_int64());
  BOOST_CHECK_EQUAL(xxx.value()[0].at("aaa").as_int64(), 42);

}

BOOST_AUTO_TEST_CASE( badArray )
{
  cout << "=== badArray ===" << endl;
  
  json j = {
    { "xxx", "yyyy" }
  };
  auto xxx = Json::getArray(j, "xxx");
  BOOST_CHECK(!xxx);

}

BOOST_AUTO_TEST_CASE( goodObject )
{
  cout << "=== goodObject ===" << endl;
  
  json j = {
    { "xxx", {
      { "aaa", 42 }
    } }
  };
  auto xxx = Json::getObject(j, "xxx");
  BOOST_CHECK(xxx);
  BOOST_CHECK(xxx.value().is_object());
  BOOST_CHECK(xxx.value().at("aaa").is_int64());
  BOOST_CHECK_EQUAL(xxx.value().at("aaa").as_int64(), 42);

}

BOOST_AUTO_TEST_CASE( badObject )
{
  cout << "=== badObject ===" << endl;
  
  json j = {
    { "xxx", "yyyy" }
  };
  auto xxx = Json::getObject(j, "xxx");
  BOOST_CHECK(!xxx);

}

BOOST_AUTO_TEST_CASE( badDate )
{
  cout << "=== badDate ===" << endl;
  
  json date = {
    { "$xxx", 1719816879599 }
  };

  BOOST_CHECK_EQUAL(Json::toISODate(date), "bad_object");
  
}

BOOST_AUTO_TEST_CASE( appendArray )
{
  cout << "=== appendArray ===" << endl;
  
  boost::json::array a = {"a", "b", "c"};
  boost::json::object obj = {
    { "x", a }
  };

  BOOST_CHECK(!Json::appendArray(&obj, "x", "a"));
  BOOST_CHECK(!Json::appendArray(&obj, "x", "b"));
  BOOST_CHECK(!Json::appendArray(&obj, "x", "c"));
  BOOST_CHECK(Json::appendArray(&obj, "x", "d"));
  BOOST_CHECK(Json::appendArray(&obj, "y", "e"));
  
  int xlen = obj.at("x").as_array().size();
  BOOST_CHECK_EQUAL(xlen, 4);
  BOOST_CHECK_EQUAL(obj.at("x").as_array()[xlen-1], "d");
  
  BOOST_CHECK(obj.if_contains("y"));
  BOOST_CHECK(obj.at("y").is_array());
  BOOST_CHECK_EQUAL(obj.at("y").as_array().size(), 1);
  BOOST_CHECK_EQUAL(obj.at("y").as_array()[0], "e");
  
}

BOOST_AUTO_TEST_CASE( arrayHas )
{
  cout << "=== arrayHas ===" << endl;
  
  boost::json::array a = {"a", "b", "c"};
  boost::json::object obj = {
    { "x", a }
  };

  BOOST_CHECK(Json::arrayHas(obj, "x", "a"));
  BOOST_CHECK(!Json::arrayHas(obj, "x", "d"));
  
}

BOOST_AUTO_TEST_CASE( arrayTail )
{
  cout << "=== arrayTail ===" << endl;
  
  boost::json::array a = {"a", "b", "c"};
  boost::json::object obj = {
    { "x", a },
    { "y", "" }
  };

  string s;
  BOOST_CHECK(Json::arrayTail(obj, "x", &s));
  BOOST_CHECK_EQUAL(s, "c");
  BOOST_CHECK(!Json::arrayTail(obj, "y", &s));
  BOOST_CHECK(!Json::arrayTail(obj, "z", &s));
  
}

BOOST_AUTO_TEST_CASE( setIndexString )
{
  cout << "=== setIndexString ===" << endl;

  boost::json::object j = {
    { "num", 42 }
  };
  j["parent"] = "a string";
  auto str = Json::getString(j, "parent");
  BOOST_CHECK(str);
  BOOST_CHECK_EQUAL(str.value(), "a string");
  
}

