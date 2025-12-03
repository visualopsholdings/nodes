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
  
  long size = filesystem::file_size(fn);
  ifstream file(fn, ios::binary);
  BOOST_CHECK(file.is_open());

  vector<char> data;
  Bin::createFileMsg(file, &data, type, id, uuid, 0, size);
  file.close();
  
  BOOST_CHECK_EQUAL(data.size(), size + Bin::headerLength());

  Bin binary(data.data(), data.size());
  
  BOOST_CHECK(binary.isBinary());
  BOOST_CHECK_EQUAL(binary.msgNum(), 0);
  
  BOOST_CHECK_EQUAL(binary.getType(), type);
  BOOST_CHECK_EQUAL(binary.getID(), id);
  BOOST_CHECK_EQUAL(binary.getUUID(), uuid);

  BOOST_CHECK(binary.isFinished());

  BOOST_CHECK_EQUAL(binary.writeFile("test.bin"), size);
  
}

BOOST_AUTO_TEST_CASE( createFileErrMsg )
{
  cout << "=== createFileErrMsg ===" << endl;
  
  string type = "test";
  string id = "667d0baedfb1ed18430d8ed3";
  string uuid = "eeeeeeee-ffff-gggg-hhhhhhhh";
  string msg = "got an error";
  
  vector<char> data;
  Bin::createFileErrMsg(&data, type, id, uuid, msg);
  
  BOOST_CHECK_EQUAL(data.size(), msg.size() + Bin::errHeaderLength());

  Bin binary(data.data(), data.size());
  auto err = binary.getError();
  BOOST_CHECK(err);
  BOOST_CHECK_EQUAL(*err, msg);
  
}

BOOST_AUTO_TEST_CASE( chunkFile )
{
  cout << "=== chunkFile ===" << endl;
  
  string type = "test";
  string id = "667d0baedfb1ed18430d8ed3";
  string uuid = "eeeeeeee-ffff-gggg-hhhhhhhh";
  string fn = "../test/media/lorem.txt";
  
  long size = filesystem::file_size(fn);
  cout << size << endl;
  
  long chunksize = 2048;
  
  {
    ifstream file(fn, ios::binary);
    BOOST_CHECK(file.is_open());
  
    vector<char> data;
    Bin::createFileMsg(file, &data, type, id, uuid, 0, chunksize);
    file.close();

    Bin binary(data.data(), data.size());
    BOOST_CHECK_EQUAL(binary.writeFile("test.txt"), chunksize);
  }
  
  {
    ifstream file(fn, ios::binary);
    BOOST_CHECK(file.is_open());
  
    vector<char> data;
    Bin::createFileMsg(file, &data, type, id, uuid, chunksize, size-chunksize);
    file.close();

    Bin binary(data.data(), data.size());
    BOOST_CHECK_EQUAL(binary.writeFile("test.txt"), size-chunksize);
  }
  
}
