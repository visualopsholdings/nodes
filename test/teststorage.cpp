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
#include "date.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace bsoncxx::builder::basic;
using namespace nodes;

string tracy = "667d0baedfb1ed18430d8ed3";
string leanne = "667d0baedfb1ed18430d8ed4";

void dbSetup() {

  Storage::instance()->init("mongodb://127.0.0.1:27017", "dev");

  User().deleteMany({{}});
  BOOST_CHECK(User().insert({
    { "_id", { { "$oid", tracy } } },
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" },
    // 2024-07-25T06:54:39.599+00:00
    { "modifyDate", { { "$date", 1721890479599 } } }
  }));
  BOOST_CHECK(User().insert({
    { "_id", { { "$oid", leanne } } },
    { "name", "leanne" },
    { "admin", false },
    { "fullname", "Leanne" },
    // 2024-09-18T11:11:30.2+00:00
    { "modifyDate", { { "$date", 1726657890002 } } }
  }));
}

BOOST_AUTO_TEST_CASE( notInit )
{
  cout << "=== notInit ===" << endl;

  BOOST_CHECK(!User().deleteById(tracy));
  
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
    { "modifyDate", Storage::instance()->getNow() },
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

BOOST_AUTO_TEST_CASE( findByIdRangeBeforeDate )
{
  cout << "=== findByIdRangeBeforeDate ===" << endl;

  dbSetup();

  auto t = Date::fromISODate("2024-07-26T00:00:00.0+00:00");
//  cout << t << endl;
  auto q = SchemaImpl::idRangeAfterDateQuery({ tracy }, "2024-07-24T00:00:00.0+00:00");

  auto results = SchemaImpl::findGeneral("users", q, {});
  BOOST_CHECK(results);
  auto users = results->values();
  BOOST_CHECK(users);
  BOOST_CHECK_EQUAL(users.value().size(), 1);
  
}

BOOST_AUTO_TEST_CASE( countGeneral )
{
  cout << "=== countGeneral ===" << endl;

  dbSetup();

  BOOST_CHECK_EQUAL(SchemaImpl::countGeneral("users", {{}}), 2);
  
}

BOOST_AUTO_TEST_CASE( existsGeneral )
{
  cout << "=== existsGeneral ===" << endl;

  dbSetup();

  BOOST_CHECK(SchemaImpl::existsGeneral("users", tracy));
  BOOST_CHECK(!SchemaImpl::existsGeneral("users", "667d0baedfb1ed18430d8eda"));
  
}







