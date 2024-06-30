/*
  teststorage.cpp
  
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

void dbSetup() {

  Storage::instance()->init("mongodb://127.0.0.1:27017", "dev");

  User().deleteMany({{}});
  BOOST_CHECK(User().insert({
    { "_id", { { "$oid", "667d0baedfb1ed18430d8ed3" } } },
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" }
  }));
  BOOST_CHECK(User().insert({
    { "_id", { { "$oid", "667d0baedfb1ed18430d8ed4" } } },
    { "name", "leanne" },
    { "admin", false },
    { "fullname", "Leanne" }
  }));
}

BOOST_AUTO_TEST_CASE( user )
{
  cout << "=== user ===" << endl;
  
  dbSetup();
  
  auto doc = User().find({{ "name", "tracy" }}, {"id", "name"}).value();
//  cout << doc.value().j() << endl;
  BOOST_CHECK(doc);
  BOOST_CHECK_EQUAL(doc->name(), "tracy");
  BOOST_CHECK_EQUAL(doc->id().size(), 24);
  cout << doc->id() << endl;
  
}

BOOST_AUTO_TEST_CASE( findAll )
{
  cout << "=== findAll ===" << endl;
  
  dbSetup();

  auto doc = User().find(json{{}}, {"id"}).values();
  BOOST_CHECK(doc);
//  cout << doc.value().j() << endl;
  BOOST_CHECK_EQUAL(doc.value().size(), 2);
  BOOST_CHECK_EQUAL(doc.value()[0].id().size(), 24);
  BOOST_CHECK_EQUAL(doc.value()[1].id().size(), 24);
  
}

BOOST_AUTO_TEST_CASE( findById )
{
  cout << "=== findById ===" << endl;
  
  dbSetup();

  auto doc = User().findById("667d0baedfb1ed18430d8ed3", {"name"}).value();
  BOOST_CHECK(doc);
//  cout << doc.value().j() << endl;
  BOOST_CHECK_EQUAL(doc->name(), "tracy");
  
}

BOOST_AUTO_TEST_CASE( findByIds )
{
  cout << "=== findByIds ===" << endl;
  
  dbSetup();

  auto doc = User().findByIds({"667d0baedfb1ed18430d8ed3", "667d0baedfb1ed18430d8ed4"}, {"name"}).values();
  BOOST_CHECK(doc);
//  cout << doc.value() << endl;
  BOOST_CHECK_EQUAL(doc.value().size(), 2);
  
}

