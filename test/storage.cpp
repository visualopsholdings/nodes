/*
  storage.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include "storage.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

// only one can be.
Storage storage;

BOOST_AUTO_TEST_CASE( user )
{
  cout << "=== user ===" << endl;
  
  auto doc = storage.coll("users").find({ { "name", "tracy" }}).value();
  BOOST_CHECK(doc);
  BOOST_CHECK((*doc).is_object());
  BOOST_CHECK((*doc).as_object().if_contains("_id"));
  BOOST_CHECK((*doc).at("_id").is_object());
  BOOST_CHECK((*doc).at("_id").as_object().if_contains("$oid"));
  string id = boost::json::value_to<string>((*doc).at("_id").at("$oid"));
  cout << id << endl;
  
}

BOOST_AUTO_TEST_CASE( findAll )
{
  cout << "=== findAll ===" << endl;
  
  auto doc = storage.coll("users").find().values();
  BOOST_CHECK(doc);
  BOOST_CHECK((*doc).is_array());
  BOOST_CHECK_EQUAL((*doc).as_array().size(), 1);
  BOOST_CHECK((*doc).as_array()[0].at("_id").is_object());
  BOOST_CHECK((*doc).as_array()[0].at("_id").as_object().if_contains("$oid"));
  
}

BOOST_AUTO_TEST_CASE( badCollection )
{
  cout << "=== badCollection ===" << endl;
  
  auto doc = storage.coll("usersx").find({ { "name", "tracy" }}).value();
  BOOST_CHECK(!doc);
  
}

