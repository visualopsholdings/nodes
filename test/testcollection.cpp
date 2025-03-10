/*
  testcollection.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 4-Mar-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include <iostream>

#include "storage/collectioni.hpp"
#include "data.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace nodes;

void checkId(boost::json::value &j) {

  BOOST_CHECK(j.is_object());
  BOOST_CHECK(j.as_object().if_contains("id"));
  BOOST_CHECK(j.at("id").is_string());
  
}

BOOST_AUTO_TEST_CASE( goodIds )
{
  cout << "=== goodIds ===" << endl;
  
  CollectionImpl impl;

  Data obj = {
    { "_id", {
        { "$oid", "yyyy" }
      }
    }
  };
  Data arr =
  {
    {
      { "_id", {
          { "$oid", "aaaa" }
        }
      }
    },
    {
      { "_id", {
          { "$oid", "bbbb" }
        }
      }
    },
    // test for non objects in the array
    "cccc",
    42
  };
  auto doc = impl.fixObjects({
    { "_id", {
        { "$oid", "xxxx" }
      }
    },
    { "obj", obj },
    { "arr", arr }
  });
//  cout << doc << endl;
  checkId(doc);
  BOOST_CHECK_EQUAL(doc.at("id").as_string(), "xxxx");
  
  BOOST_CHECK(doc.as_object().if_contains("obj"));
  auto subobj = doc.at("obj");
  checkId(subobj);
  BOOST_CHECK_EQUAL(subobj.at("id").as_string(), "yyyy");
  
  BOOST_CHECK(doc.as_object().if_contains("arr"));
  BOOST_CHECK(doc.at("arr").is_array());
  auto subarr = doc.at("arr").as_array();
  BOOST_CHECK_EQUAL(subarr.size(), 4);
  checkId(subarr[0]);
  BOOST_CHECK_EQUAL(subarr[0].at("id").as_string(), "aaaa");
  checkId(subarr[1]);
  BOOST_CHECK_EQUAL(subarr[1].at("id").as_string(), "bbbb");
  BOOST_CHECK_EQUAL(subarr[2].as_string(), "cccc");
  BOOST_CHECK_EQUAL(subarr[3].as_int64(), 42);

}

BOOST_AUTO_TEST_CASE( badIds )
{
  cout << "=== badIds ===" << endl;
  
  CollectionImpl impl;

  auto doc = impl.fixObjects({
    { "_id", "xxxx" }
  });
//  cout << doc << endl;
  BOOST_CHECK(doc.is_object());
  BOOST_CHECK(doc.at("_id").is_string());
  BOOST_CHECK_EQUAL(doc.at("_id").as_string(), "xxxx");
  
}

BOOST_AUTO_TEST_CASE( noOID )
{
  cout << "=== noOID ===" << endl;
  
  CollectionImpl impl;

  auto doc = impl.fixObjects({
    { "_id", {
        { "string", "xxxx" }
      }
    }
  });
//  cout << doc << endl;
  BOOST_CHECK(doc.is_object());
  BOOST_CHECK(doc.at("_id").is_object());
  
}

BOOST_AUTO_TEST_CASE( goodDate )
{
  cout << "=== goodDate ===" << endl;
  
  CollectionImpl impl;

  auto doc = impl.fixObjects({
    { "modifyDate", {
        { "$date", 1722068626483 }
      }
    }
  });
//  cout << doc << endl;
  BOOST_CHECK(doc.is_object());
  BOOST_CHECK(doc.at("modifyDate").is_string());
  BOOST_CHECK_EQUAL(doc.at("modifyDate").as_string(), "2024-07-27T08:23:46.483+00:00");
  
}

BOOST_AUTO_TEST_CASE( number )
{
  cout << "=== number ===" << endl;
  
  CollectionImpl impl;

  auto doc = impl.fixObjects({
    { "_id", {
        { "$oid", "xxxx" }
      }
    },
    { "name", "Collection 1" },
    { "stuff", 2048 }
  });
//  cout << doc << endl;
  BOOST_CHECK(doc.is_object());
  
}



