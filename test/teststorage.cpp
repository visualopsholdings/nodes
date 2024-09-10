/*
  teststorage.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include <iostream>

#include "storage.hpp"
#include "json.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

string tracy = "667d0baedfb1ed18430d8ed3";
string leanne = "667d0baedfb1ed18430d8ed4";

void dbSetup() {

  Storage::instance()->init("mongodb://127.0.0.1:27017", "dev");

  User().deleteMany({{}});
  BOOST_CHECK(User().insert({
    { "_id", { { "$oid", tracy } } },
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" }
  }));
  BOOST_CHECK(User().insert({
    { "_id", { { "$oid", leanne } } },
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

  auto doc = User().findById(tracy, {"name"}).value();
  BOOST_CHECK(doc);
//  cout << doc.value().j() << endl;
  BOOST_CHECK_EQUAL(doc->name(), "tracy");
  
}

BOOST_AUTO_TEST_CASE( findByIds )
{
  cout << "=== findByIds ===" << endl;
  
  dbSetup();

  auto docs = User().findByIds({tracy, leanne}, {"name"}).values();
  BOOST_CHECK(docs);
//  cout << doc.value() << endl;
  BOOST_CHECK_EQUAL(docs.value().size(), 2);
  
  BOOST_CHECK(find_if(docs.value().begin(), docs.value().end(), 
    [](auto &e) { return e.name() == "tracy"; }) != docs.value().end());
  BOOST_CHECK(find_if(docs.value().begin(), docs.value().end(), 
    [](auto &e) { return e.name() == "xxx"; }) == docs.value().end());
  
}

BOOST_AUTO_TEST_CASE( insertExisting )
{
  cout << "=== insertExisting ===" << endl;
  
  dbSetup();

  auto result = User().insert({
    { "_id", { { "$oid", tracy } } },
    { "name", "tracy2" }
  });
  BOOST_CHECK(result);
  BOOST_CHECK_EQUAL(result.value(), "exists");

}

BOOST_AUTO_TEST_CASE( insertNew )
{
  cout << "=== insertNew ===" << endl;
  
  dbSetup();

  auto result = User().insert({
    { "name", "bob" }
  });
  BOOST_CHECK(result);
  BOOST_CHECK_EQUAL(result.value().size(), 24);

}

BOOST_AUTO_TEST_CASE( update )
{
  cout << "=== update ===" << endl;
  
  dbSetup();

  auto result = User().updateById(tracy, {
    { "name", "tracy2" },
    { "fullname", "Tracy new" }
  });
  BOOST_CHECK(result);

}

BOOST_AUTO_TEST_CASE( rawUpdate )
{
  cout << "=== rawUpdate ===" << endl;
  
  dbSetup();

  auto result = User().rawUpdateById(tracy, {
    { "$set", { { "name", "tracy2" } } }
  });
  BOOST_CHECK(result);

}

BOOST_AUTO_TEST_CASE( bulkInsert )
{
  cout << "=== bulkInsert ===" << endl;
  
  dbSetup();

  boost::json::array a =
  {
    {
      { "_id", tracy },
      { "name", "tracy2" }
    },
    {
      { "_id", leanne },
      { "name", "leanne2" }
    }      
  };
  cout << a << endl;
  BOOST_CHECK(Storage::instance()->bulkInsert("users", a));

}

BOOST_AUTO_TEST_CASE( getNow )
{
  cout << "=== getNow ===" << endl;

  json date = Storage::instance()->getNow();
  string dates = Json::toISODate(date);
  
  // get the year a different way and compare with the start.
  std::time_t t = std::time(nullptr);
  std::tm *const tm = std::localtime(&t);
  stringstream ss;
  ss << 1900 + tm->tm_year;
  BOOST_CHECK_EQUAL(dates.substr(0, 4), ss.str());

}

BOOST_AUTO_TEST_CASE( deleteById )
{
  cout << "=== deleteById ===" << endl;

  dbSetup();

  BOOST_CHECK(User().deleteById(tracy));
  BOOST_CHECK(!User().deleteById(tracy));
  
}



