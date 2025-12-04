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

int processChunk(const string &fn, const string &outfile, long offset, long size) {

  ifstream file(fn, ios::binary);
  BOOST_CHECK(file.is_open());

  vector<char> data;
  Bin::createFileMsg(file, &data, "test", "667d0baedfb1ed18430d8ed3", "eeeeeeee-ffff-gggg-hhhhhhhh", offset, size);
  file.close();

  Bin binary(data.data(), data.size());
  return binary.writeFile(outfile);
  
}

bool indenticalFiles(const string &fn1, const string &fn2) {

  ifstream file1(fn1, ios::binary);
  ifstream file2(fn2, ios::binary);
  if (!file1.is_open() || !file2.is_open()) {
    cerr << "Error opening one or both files." << endl;
    return false;
  }

  char byte1, byte2;
  long long offset = 0;
  bool identical = true;
  while (true) {
    file1.read(&byte1, 1);
    file2.read(&byte2, 1);
    if (file1.eof() || file2.eof()) {
      break;
    }
    if (byte1 != byte2) {
      std::cout << "Difference found at offset " << offset << ": "
                << "File 1: 0x" << std::hex << (static_cast<int>(byte1) & 0xFF) << " "
                << "File 2: 0x" << std::hex << (static_cast<int>(byte2) & 0xFF) << std::dec << std::endl;
      identical = false;
    }
    offset++;
  }
  
  // Check if one file is longer than the other
  if (file1.eof() && !file2.eof()) {
    std::cout << "File 1 is shorter than File 2." << std::endl;
    identical = false;
  } else if (!file1.eof() && file2.eof()) {
    std::cout << "File 2 is shorter than File 1." << std::endl;
    identical = false;
  }
  
  file1.close();
  file2.close();
    
  return identical;
  
}

BOOST_AUTO_TEST_CASE( chunkFile )
{
  cout << "=== chunkFile ===" << endl;
  
  string fn = "../test/media/lorem.txt";
  
  long size = filesystem::file_size(fn);
//  cout << size << endl;
  
  long chunksize = 100;
  
  // do all the whole chunks.
  long offset = 0;
  while (true) {
//    cout << offset << endl;
    int wsize = processChunk(fn, "test.txt", offset, chunksize);
    if (wsize < chunksize) {
      break;
    }
    offset += chunksize;
  }
  BOOST_CHECK(indenticalFiles(fn, "test.txt"));
  
}
