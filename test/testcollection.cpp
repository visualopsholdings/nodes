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
#include "dict.hpp"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace nodes;
using namespace vops;

void checkId(boost::json::value &j) {

  BOOST_CHECK(j.is_object());
  BOOST_CHECK(j.as_object().if_contains("id"));
  BOOST_CHECK(j.at("id").is_string());
  
}

void checkId(const DictG &g) {

  auto o = Dict::getObject(g);
  BOOST_CHECK(o);
  BOOST_CHECK(Dict::getString(*o, "id"));
  
}

BOOST_AUTO_TEST_CASE( goodIds )
{
  cout << "=== goodIds ===" << endl;
  
  CollectionImpl impl;

  DictO obj = dictO({
    { "_id", dictO({
        { "$oid", "yyyy" }
      })
    }
  });
  DictV arr =
  {
    dictO({
      { "_id", dictO({
          { "$oid", "aaaa" }
        })
      }
    }),
    dictO({
      { "_id", dictO({
          { "$oid", "bbbb" }
        })
      }
    }),
    // test for non objects in the array
    "cccc",
    42
  };
  auto doc = impl.fixObjects(dictO({
    { "_id", dictO({
        { "$oid", "xxxx" }
      })
    },
    { "obj", obj },
    { "arr", arr }
  }));
//  cout << doc << endl;
  checkId(doc);
  BOOST_CHECK_EQUAL(*Dict::getString(doc, "id"), "xxxx");
  
  auto subobj = Dict::getObject(doc, "obj");
  checkId(*subobj);
  BOOST_CHECK_EQUAL(*Dict::getString(*subobj, "id"), "yyyy");
  
  auto subarr = Dict::getVector(doc, "arr");
  BOOST_CHECK(subarr);
  BOOST_CHECK_EQUAL(subarr->size(), 4);
  checkId((*subarr)[0]);
  BOOST_CHECK_EQUAL(*Dict::getStringG((*subarr)[0], "id"), "aaaa");
  checkId((*subarr)[1]);
  BOOST_CHECK_EQUAL(*Dict::getStringG((*subarr)[1], "id"), "bbbb");
  BOOST_CHECK_EQUAL(*Dict::getString((*subarr)[2]), "cccc");
  BOOST_CHECK_EQUAL(*Dict::getNum((*subarr)[3]), 42);

}

BOOST_AUTO_TEST_CASE( badIds )
{
  cout << "=== badIds ===" << endl;
  
  CollectionImpl impl;

  auto doc = impl.fixObjects(dictO({
    { "_id", "xxxx" }
  }));
//   cout << Dict::toString(doc) << endl;
  BOOST_CHECK_EQUAL(*Dict::getString(doc, "_id"), "xxxx");
  
}

BOOST_AUTO_TEST_CASE( noOID )
{
  cout << "=== noOID ===" << endl;
  
  CollectionImpl impl;

  auto doc = impl.fixObjects(dictO({
    { "_id", dictO({
        { "string", "xxxx" }
      })
    }
  }));
//  cout << doc << endl;
  BOOST_CHECK(Dict::getObject(doc, "_id"));
  
}

BOOST_AUTO_TEST_CASE( goodDate )
{
  cout << "=== goodDate ===" << endl;
  
  CollectionImpl impl;

  auto doc = impl.fixObjects(dictO({
    { "modifyDate", dictO({
        { "$date", 1722068626483 }
      })
    }
  }));
//  cout << doc << endl;
  auto mod = Dict::getString(doc, "modifyDate");
  BOOST_CHECK(mod);
  BOOST_CHECK_EQUAL(*mod, "2024-07-27T08:23:46.483+00:00");
  
}

BOOST_AUTO_TEST_CASE( number )
{
  cout << "=== number ===" << endl;
  
  CollectionImpl impl;

  auto doc = impl.fixObjects(dictO({
    { "_id", dictO({
        { "$oid", "xxxx" }
      })
    },
    { "name", "Collection 1" },
    { "stuff", 2048 }
  }));
//  cout << doc << endl;
  BOOST_CHECK_EQUAL(*Dict::getNum(doc, "stuff"), 2048);
  
}



