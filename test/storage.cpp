/*
  storage.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include <iostream>

#include "storage.hpp"
#include "schema.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

// only one can be.
Storage storage;

BOOST_AUTO_TEST_CASE( user )
{
  cout << "=== user ===" << endl;
  
  auto doc = User(storage).find({ { "name", "tracy" }}).value();
  BOOST_CHECK(doc);
  BOOST_CHECK(doc.value().is_object());
  BOOST_CHECK(doc.value().as_object().if_contains("_id"));
  BOOST_CHECK(doc.value().at("_id").is_object());
  BOOST_CHECK(doc.value().at("_id").as_object().if_contains("$oid"));
  string id = boost::json::value_to<string>(doc.value().at("_id").at("$oid"));
  cout << id << endl;
  
}

BOOST_AUTO_TEST_CASE( findAll )
{
  cout << "=== findAll ===" << endl;
  
  auto doc = User(storage).find().values();
  BOOST_CHECK(doc);
  BOOST_CHECK(doc.value().is_array());
  BOOST_CHECK_EQUAL(doc.value().as_array().size(), 1);
  BOOST_CHECK(doc.value().as_array()[0].at("_id").is_object());
  BOOST_CHECK(doc.value().as_array()[0].at("_id").as_object().if_contains("$oid"));
  
}
