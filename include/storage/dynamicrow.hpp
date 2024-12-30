/*
  dynamicrow.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 30-June-2024
    
  A row that contains JSON for Nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_dynamicrow
#define H_dynamicrow

#include "data.hpp"

using namespace std;

namespace nodes {

class Data;

class DynamicRow {

public:
  DynamicRow(Data data): _data(data) {}
  
  Data d() { return _data; }
  string id() { return getString("id"); }
  string getString(const string &name) const;
  vector<string> getStringArray(const string &name);
  Data getData(const string &name);
  bool getBool(const string &name, bool silent=false) const;
  int getNumber(const string &name) const;
  
private:
  Data _data;
};

} // nodes

#endif // H_dynamicrow
