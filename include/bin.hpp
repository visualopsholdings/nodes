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

using namespace std;

namespace nodes {

class Bin {
 
public:

  static bool isBinary(const char *data, size_t size);
  static unsigned char msgNum(const char *data, size_t size);
 
  static void createFileMsg(ifstream &file, vector<char> *data, unsigned char msg, const string &coll, const string &id, const string &uuid, long offset, long size);
  static bool fileMsgDetails(const char *data, size_t size, string *type, string *id, string *uuid);
  static size_t writeFileMsg(const string &fn, const char *data, size_t size);
  static bool finishedFile(const char *data, size_t size);
 
  // for tests.
  static int headerLength();
  
private:
  static void addNum(vector<char> *data, long n);
  static long getNum(const char *data, size_t offset);
  static void addType(vector<char> *data, const string &type);
 
};

} // nodes

#endif // H_bin
