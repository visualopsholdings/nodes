/*
  bin.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Dec-2025
    
  Binary data code for nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_bin
#define H_bin

#include <fstream>
#include <vector>
#include <optional>

using namespace std;

namespace nodes {

class Bin {
 
public:

  Bin(const char *data, size_t size): _data(data), _size(size) {}
  Bin(): _data(0), _size(0) {}
  
  bool isBinary();
  unsigned char msgNum();
  
  // binary FILE specific
  static void createFileMsg(ifstream &file, vector<char> *data, const string &type, const string &id, const string &uuid, bool fullscan, long offset, long size);
  static void createFileTooLargeMsg(vector<char> *data, const string &type, const string &id, const string &uuid, bool fullscan, long size);
  static void createFileErrMsg(vector<char> *data, const string &type, const string &id, const string &uuid, bool fullscan, const string &err);
  
  string getType();
  string getID();
  string getUUID();
  long getOffset();
  size_t writeFile(const string &fn);
  bool isFinished();
  bool isTooLarge();
  bool isFullScan();
  optional<string> getError();

  void dump();

  // for tests.
  static int headerLength();
  static int errHeaderLength();
  
  // the value of the binStatus field in an object with a binary attachment.
  static const int NEEDS_DOWNLOAD;
  static const int DOWNLOADED;
  static const int TOO_LARGE;
  static const int DOWNLOAD_ERR;
 
private:
  const char *_data;
  size_t _size;
  
  long getNum(size_t offset);
  bool testFlags(unsigned char flag);

  static void createFileHeader(vector<char> *data, const string &type, const string &id, const string &uuid, unsigned char flags);
  static void addNum(vector<char> *data, long n);
  static void addType(vector<char> *data, const string &type);
 
};

} // nodes

#endif // H_bin
