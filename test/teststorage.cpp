/*
  teststorage.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-May-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include <iostream>

#include "storage.hpp"
#include "date.hpp"
#include "dict.hpp"

#include <bsoncxx/json.hpp>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace nodes;
using namespace vops;

string tracy = "667d0baedfb1ed18430d8ed3";
string leanne = "667d0baedfb1ed18430d8ed4";

void dbSetup() {

  Storage::instance()->init("mongodb://127.0.0.1:27017", "dev");

  User().deleteMany(dictO({}));
  BOOST_CHECK(User().insert(dictO({
    { "_id", dictO({{ "$oid", tracy }}) },
    { "name", "tracy" },
    { "admin", true },
    { "fullname", "Tracy" },
    // 2024-07-25T06:54:39.599+00:00
    { "modifyDate", dictO({{ "$date", 1721890479599 }}) }
  })));
  BOOST_CHECK(User().insert(dictO({
    { "_id", dictO({{ "$oid", leanne }}) },
    { "name", "leanne" },
    { "admin", false },
    { "fullname", "Leanne" },
    // 2024-09-18T11:11:30.2+00:00
    { "modifyDate", dictO({{ "$date", 1726657890002 }}) }
  })));
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
  
  auto doc = User().find(dictO({{ "name", "tracy" }}), {"id", "name"}).one();
  BOOST_CHECK(doc);
//  cout << Dict::toString(doc->dict()) << endl;
  BOOST_CHECK_EQUAL(doc->name(), "tracy");
  BOOST_CHECK_EQUAL(doc->id().size(), 24);
  cout << doc->id() << endl;
  
}

BOOST_AUTO_TEST_CASE( findAll )
{
  cout << "=== findAll ===" << endl;
  
  dbSetup();

  auto doc = User().find(dictO({}), {"id"}).all();
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

  auto doc = User().findById(tracy, {"name"}).one();
  BOOST_CHECK(doc);
//  cout << doc.value().j() << endl;
  BOOST_CHECK_EQUAL(doc->name(), "tracy");
  
}

BOOST_AUTO_TEST_CASE( findByIds )
{
  cout << "=== findByIds ===" << endl;
  
  dbSetup();

  auto docs = User().findByIds({tracy, leanne}, {"name"}).all();
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

  auto result = User().insert(dictO({
    { "_id", dictO({{ "$oid", tracy }}) },
    { "name", "tracy2" }
  }));
  BOOST_CHECK(result);
  BOOST_CHECK_EQUAL(result.value(), "exists");

}

BOOST_AUTO_TEST_CASE( insertNew )
{
  cout << "=== insertNew ===" << endl;
  
  dbSetup();

  auto result = User().insert(dictO({
    { "name", "bob" }
  }));
  BOOST_CHECK(result);
  BOOST_CHECK_EQUAL(result.value().size(), 24);

}

BOOST_AUTO_TEST_CASE( update )
{
  cout << "=== update ===" << endl;
  
  dbSetup();

  auto result = User().updateById(tracy, dictO({
    { "name", "tracy2" },
    { "modifyDate", Storage::instance()->getNow() },
    { "fullname", "Tracy new" }
  }));
  BOOST_CHECK(result);

}

BOOST_AUTO_TEST_CASE( rawUpdate )
{
  cout << "=== rawUpdate ===" << endl;
  
  dbSetup();

  auto result = User().rawUpdateById(tracy, dictO({
    { "$set", dictO({{ "name", "tracy2" }}) }
  }));
  BOOST_CHECK(result);

}

BOOST_AUTO_TEST_CASE( bulkInsert )
{
  cout << "=== bulkInsert ===" << endl;
  
  dbSetup();

  vector<DictO> a =
  {
    dictO({
      { "_id", tracy },
      { "name", "tracy2" }
    }),
    dictO({
      { "_id", leanne },
      { "name", "leanne2" }
    }) 
  };
//  cout << Dict::toString(a) << endl;
  BOOST_CHECK(Storage::instance()->bulkInsert("users", a));

}

BOOST_AUTO_TEST_CASE( getNow )
{
  cout << "=== getNow ===" << endl;

  auto date = Storage::instance()->getNow();
  string dates = CollectionImpl::toISODate(date);
  
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

#ifdef MONGO_DB
BOOST_AUTO_TEST_CASE( findByIdRangeBeforeDate )
{
  cout << "=== findByIdRangeBeforeDate ===" << endl;

  dbSetup();

  auto t = Date::fromISODate("2024-07-26T00:00:00.0+00:00");
//  cout << t << endl;
  auto q = SchemaImpl::idRangeAfterDateQuery({{ tracy }}, "2024-07-24T00:00:00.0+00:00");

  auto results = SchemaImpl::findGeneral("users", q, {});
  BOOST_CHECK(results);
  auto users = results->all();
  BOOST_CHECK(users);
  BOOST_CHECK_EQUAL(users.value().size(), 1);
  
}
#endif

BOOST_AUTO_TEST_CASE( countGeneral )
{
  cout << "=== countGeneral ===" << endl;

  dbSetup();

  BOOST_CHECK_EQUAL(SchemaImpl::countGeneral("users", dictO({})), 2);
  
}

BOOST_AUTO_TEST_CASE( existsGeneral )
{
  cout << "=== existsGeneral ===" << endl;

  dbSetup();

  BOOST_CHECK(SchemaImpl::existsGeneral("users", tracy));
  BOOST_CHECK(!SchemaImpl::existsGeneral("users", "667d0baedfb1ed18430d8eda"));
  
}

BOOST_AUTO_TEST_CASE( findWithLimit )
{
  cout << "=== findWithLimit ===" << endl;

  dbSetup();

  auto results = SchemaImpl::findGeneral("users", dictO({}), {}, 1);
  BOOST_CHECK(results);
  auto users = results->all();
  BOOST_CHECK(users);
  BOOST_CHECK_EQUAL(users.value().size(), 1);
  
}

BOOST_AUTO_TEST_CASE( findLatest )
{
  cout << "=== findLatest ===" << endl;

  dbSetup();

  auto results = SchemaImpl::findGeneral("users", dictO({}), {}, 1, dictO({{ "modifyDate", -1 }}));
  BOOST_CHECK(results);
  auto users = results->all();
  BOOST_CHECK(users);
  BOOST_CHECK_EQUAL(users.value().size(), 1);
  auto n = users.value().begin();
  BOOST_CHECK_EQUAL(Dict::getStringG(*n, "name").value(), "leanne");
  
}

BOOST_AUTO_TEST_CASE( convertToBSON )
{
  cout << "=== convertToBSON ===" << endl;
 
  auto doc = ResultImpl::convert(dictO({
    { "$and", DictV{
      dictO({
        { "_id", dictO({{ "$oid", tracy }}) },
      }),
      dictO({
        { "modifyDate", dictO({{ "$date", 1721890479599 }}) }
      })
    }
  }}));
  cout << bsoncxx::to_json(doc) << endl;
 
}

BOOST_AUTO_TEST_CASE( convertSetToBSON )
{
  cout << "=== convertSetToBSON ===" << endl;
 
  auto doc = ResultImpl::convert(dictO({
    { "$set", dictO({
        { "modifyDate", "2022-01-01T13:00:00.0+00:00" },
        { "name", "Shared Collection" },
        { "policy", "6386dbabddf5aaf74ca1e1f6" },
        { "upstream", true },
        { "active", true }
      })
    }
  }));

//  cout << bsoncxx::to_json(doc) << endl;
 
}

BOOST_AUTO_TEST_CASE( convertFromBSON )
{
  cout << "=== convertFromBSON ===" << endl;
 
  auto bson = ResultImpl::convert(dictO({
    { "_id", dictO({{ "$oid", tracy }}) }, 
    { "name", "tracy" },
    { "modifyDate", dictO({{ "$date", 1641042000000 }}) }, 
    { "members", DictV{
       dictO({ { "_id", dictO({{ "$oid", "6920315c0651c850ca114235" }}) }, { "user", "6121bdfaec9e5a059715739c" } }),
       dictO({ { "_id", dictO({{ "$oid", "6920315c0651c850ca114236" }}) }, { "user", "6142d258ddf5aa5644057d35" } })
      }
    } 
  }));

  auto dict = ResultImpl::convert(bson);
  BOOST_CHECK(dict);
//  cout << Dict::toString(*dict) << endl;
  
}
