/*
  bin.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Dec-2025
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "bin.hpp"
#include "log.hpp"

#include <sstream>
#include <iostream>

#define SMALLEST_BINARY   1
#define NUM_WIDTH         8
#define BIN_MARKER        'B'
#define MSG_POS           1
#define TYPE_POS          2
#define TYPE_LEN          12
#define ID_POS            (TYPE_POS+TYPE_LEN)
#define ID_LEN            24
#define UUID_POS          (ID_POS+ID_LEN)
#define UUID_LEN          27
#define OFFSET_POS        (UUID_POS+UUID_LEN)
#define FLAGS_POS         (OFFSET_POS+NUM_WIDTH)
#define DATA_POS          (FLAGS_POS+1)
#define FLAGS_FINISHED    0x1

namespace nodes {

int Bin::headerLength() {
  return DATA_POS;
}

void Bin::addNum(vector<char> *data, long n) {

  stringstream ss;
  ss << std::setw(NUM_WIDTH) << n;
  string s = ss.str();
  L_TRACE("[" << s << "]");
  copy(s.begin(), s.end(), back_inserter(*data));

}

void Bin::addType(vector<char> *data, const string &type) {

  stringstream ss;
  ss << std::setw(TYPE_LEN) << type.substr(0, TYPE_LEN);
  string s = ss.str();
  L_TRACE("[" << s << "]");
  copy(s.begin(), s.end(), back_inserter(*data));

}

void Bin::createFileMsg(ifstream &file, vector<char> *data, unsigned char msg, const string &coll, const string &id, const string &uuid, long offset, long size) {

  data->push_back(BIN_MARKER); // binary start with the letter B.
  data->push_back(msg); // the message number
  L_TRACE("TYPE at " << data->size());
  addType(data, coll);
  L_TRACE("ID at " << data->size());
  copy(id.begin(), id.end(), back_inserter(*data));
  L_TRACE("UUID at " << data->size());
  copy(uuid.begin(), uuid.end(), back_inserter(*data));
  L_TRACE("offset at " << data->size());
  addNum(data, offset); // 8 bytes
  L_TRACE("flags at " << data->size());
  data->push_back(FLAGS_FINISHED); // 1 byte flags (finished or not).
  
  // and the data.
  L_TRACE("data at " << data->size());
  // if we are asked too, we should only write from offset to size NOT the 
  // whole file.
  copy(istreambuf_iterator<char>(file), istreambuf_iterator<char>(), back_inserter(*data));

}

bool Bin::isBinary(const char *data, size_t size) {

  if (size < SMALLEST_BINARY) {
    L_ERROR("data is too tiny");
    return false;
  }
  
  return data[0] == BIN_MARKER;
  
}

unsigned char Bin::msgNum(const char *data, size_t size) {

  if (size < (MSG_POS + 1)) {
    L_ERROR("data is too tiny for message");
    return false;
  }
  
  return data[MSG_POS];

}

inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

bool Bin::fileMsgDetails(const char *data, size_t size, string *type, string *id, string *uuid) {

  if (size < (UUID_POS + UUID_LEN)) {
    L_ERROR("data is too tiny to have any details");
    return false;
  }
  
  *type = string(data+TYPE_POS, TYPE_LEN);
  ltrim(*type);
  *id = string(data+ID_POS, ID_LEN);
  *uuid = string(data+UUID_POS, UUID_LEN);

  return true;
  
}

bool Bin::finishedFile(const char *data, size_t size) {

  if (size < FLAGS_POS) {
    L_ERROR("data is too tiny to have flags");
    return false;
  }

  return data[FLAGS_POS] & FLAGS_FINISHED;
}

long Bin::getNum(const char *data, size_t offset) {

  stringstream os(string(data+offset, NUM_WIDTH));
  long num;
  os >> num;
  if (os.fail()) {
    L_ERROR("failed to parse offset " << os.str());
    return 0;
  }
  return num;
  
}

size_t Bin::writeFileMsg(const string &fn, const char *data, size_t size) {

  if (size < DATA_POS) {
    L_ERROR("data is too tiny to have file contents");
    return 0;
  }
  
  long doffset = getNum(data, OFFSET_POS);
  
  L_TRACE("ignoring offset " << doffset);
  
  ofstream fs(fn, std::ios::out | std::ios::binary);
  fs.write(data+DATA_POS, size-DATA_POS);
  fs.close();
  
  return size-DATA_POS;
  
}

} // nodes
