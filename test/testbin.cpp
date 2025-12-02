/*
  testbin.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Dec-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include <iostream>

#include "bin.hpp"

#include <filesystem>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace nodes;

BOOST_AUTO_TEST_CASE( createFileMsg )
{
  cout << "=== createFileMsg ===" << endl;
  
  string type = "test";
  string id = "667d0baedfb1ed18430d8ed3";
  string uuid = "eeeeeeee-ffff-gggg-hhhhhhhh";
  string fn = "../test/media/" + uuid;
  unsigned char msg = 1;
  
  long size = filesystem::file_size(fn);
  ifstream file(fn, ios::binary);
  BOOST_CHECK(file.is_open());

  vector<char> data;
  Bin::createFileMsg(file, &data, msg, type, id, uuid, 0, size);
  file.close();
  
  BOOST_CHECK_EQUAL(data.size(), size + Bin::headerLength());
  BOOST_CHECK(Bin::isBinary(data.data(), data.size()));
  BOOST_CHECK_EQUAL(Bin::msgNum(data.data(), data.size()), msg);
  
  string t, i, u;
  BOOST_CHECK(Bin::fileMsgDetails(data.data(), data.size(), &t, &i, &u));
  BOOST_CHECK_EQUAL(t, type);
  BOOST_CHECK_EQUAL(i, id);
  BOOST_CHECK_EQUAL(u, uuid);

  BOOST_CHECK(Bin::finishedFile(data.data(), data.size()));

  BOOST_CHECK_EQUAL(Bin::writeFileMsg("test.bin", data.data(), data.size()), size);
  
}
