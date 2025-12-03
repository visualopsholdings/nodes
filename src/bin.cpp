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
#define FLAGS_POS         2
#define TYPE_POS          (FLAGS_POS+1)
#define TYPE_LEN          12
#define ID_POS            (TYPE_POS+TYPE_LEN)
#define ID_LEN            24
#define UUID_POS          (ID_POS+ID_LEN)
#define UUID_LEN          27
#define OFFSET_POS        (UUID_POS+UUID_LEN)
#define ERR_POS           OFFSET_POS
#define DATA_POS          (OFFSET_POS+NUM_WIDTH)

#define FLAGS_FINISHED    0x01
#define FLAGS_TOOLARGE    0x02
#define FLAGS_ERROR       0x04

namespace nodes {

const int Bin::NEEDS_DOWNLOAD = 0;
const int Bin::DOWNLOADED = 1;
const int Bin::TOO_LARGE = 2;
const int Bin::DOWNLOAD_ERR = 3;

int Bin::headerLength() {
  return DATA_POS;
}

int Bin::errHeaderLength() {
  return ERR_POS;
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

void Bin::createFileMsg(ifstream &file, vector<char> *data, const string &type, const string &id, const string &uuid, long offset, long size) {

  data->push_back(BIN_MARKER); // binary start with the letter B.
  data->push_back(0); // the message number
  L_TRACE("flags at " << data->size());
  data->push_back(FLAGS_FINISHED); // 1 byte flags (finished or not).
  L_TRACE("TYPE at " << data->size());
  addType(data, type);
  L_TRACE("ID at " << data->size());
  copy(id.begin(), id.end(), back_inserter(*data));
  L_TRACE("UUID at " << data->size());
  copy(uuid.begin(), uuid.end(), back_inserter(*data));
  L_TRACE("offset at " << data->size());
  addNum(data, offset);
  
  // and the data.
  L_TRACE("data at " << data->size());
  // if we are asked too, we should only write from offset to size NOT the 
  // whole file.
  copy(istreambuf_iterator<char>(file), istreambuf_iterator<char>(), back_inserter(*data));

}

void Bin::createFileTooLargeMsg(vector<char> *data, const string &type, const string &id, const string &uuid, long size) {

  data->push_back(BIN_MARKER); // binary start with the letter B.
  data->push_back(0); // the message number
  L_TRACE("flags at " << data->size());
  data->push_back(FLAGS_TOOLARGE); // 1 byte flags (finished or not).
  L_TRACE("TYPE at " << data->size());
  addType(data, type);
  L_TRACE("ID at " << data->size());
  copy(id.begin(), id.end(), back_inserter(*data));
  L_TRACE("UUID at " << data->size());
  copy(uuid.begin(), uuid.end(), back_inserter(*data));
  L_TRACE("offset at " << data->size());
  addNum(data, 0);

}

void Bin::createFileErrMsg(vector<char> *data, const string &type, const string &id, const string &uuid, const string &err) {

  data->push_back(BIN_MARKER); // binary start with the letter B.
  data->push_back(0); // the message number
  L_TRACE("flags at " << data->size());
  data->push_back(FLAGS_ERROR); // 1 byte flags (finished or not).
  L_TRACE("TYPE at " << data->size());
  addType(data, type);
  L_TRACE("ID at " << data->size());
  copy(id.begin(), id.end(), back_inserter(*data));
  L_TRACE("UUID at " << data->size());
  copy(uuid.begin(), uuid.end(), back_inserter(*data));
  L_TRACE("err at " << data->size());
  copy(err.begin(), err.end(), back_inserter(*data));

}

bool Bin::isBinary() {

  if (_size < SMALLEST_BINARY) {
    L_ERROR("data is too tiny");
    return false;
  }
  
  return _data[0] == BIN_MARKER;
  
}

unsigned char Bin::msgNum() {

  if (_size < (MSG_POS + 1)) {
    L_ERROR("data is too tiny for message");
    return false;
  }
  
  return _data[MSG_POS];

}

inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

string Bin::getType() {

  if (_size < (TYPE_POS + TYPE_LEN)) {
    L_ERROR("data is too tiny to have a type");
    return "??";
  }
  
  string type(_data+TYPE_POS, TYPE_LEN);
  ltrim(type);
  return type;
}

string Bin::getID() {

  if (_size < (ID_POS + ID_LEN)) {
    L_ERROR("data is too tiny to have an id");
    return "??";
  }
  
  return string(_data+ID_POS, ID_LEN);
  
}

string Bin::getUUID() {

  if (_size < (UUID_POS + UUID_LEN)) {
    L_ERROR("data is too tiny to have a UUID");
    return "??";
  }
  
  return string(_data+UUID_POS, UUID_LEN);
  
}

bool Bin::isFinished() {

  if (_size < FLAGS_POS+1) {
    L_ERROR("data is too tiny to have flags");
    return false;
  }

  return _data[FLAGS_POS] & FLAGS_FINISHED;

}

bool Bin::isTooLarge() {

  if (_size < FLAGS_POS+1) {
    L_ERROR("data is too tiny to have flags");
    return false;
  }

  return _data[FLAGS_POS] & FLAGS_TOOLARGE;

}

optional<string> Bin::getError() {

  if (_size < ERR_POS) {
    L_ERROR("data is too tiny for error");
    return nullopt;
  }
  
  if (_data[FLAGS_POS] & FLAGS_ERROR) {
    return string(_data+ERR_POS, _size-ERR_POS);
  }
  
  return nullopt;
}

long Bin::getNum(size_t offset) {

  stringstream os(string(_data+offset, NUM_WIDTH));
  long num;
  os >> num;
  if (os.fail()) {
    L_ERROR("failed to parse offset " << os.str());
    return 0;
  }
  return num;
  
}

size_t Bin::writeFile(const string &fn) {

  if (_size < DATA_POS) {
    L_ERROR("data is too tiny to have file contents");
    return 0;
  }
  
  long doffset = getNum(OFFSET_POS);
  
  L_TRACE("ignoring offset " << doffset);
  
  ofstream fs(fn, std::ios::out | std::ios::binary);
  fs.write(_data+DATA_POS, _size-DATA_POS);
  fs.close();
  
  return _size-DATA_POS;
  
}

} // nodes
