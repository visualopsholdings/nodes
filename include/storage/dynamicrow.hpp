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

#include "dict.hpp"
#include "data.hpp"

using namespace std;
using vops::DictO;

namespace nodes {

class DynamicRow {

public:
  DynamicRow(DictO data): _data(data) {}
  
  DictO dict() { return _data; }
  string id() { return getString("id"); }
  string getString(const string &name) const;
  vector<string> getStringArray(const string &name);
  DictO getObject(const string &name);
  bool getBool(const string &name, bool silent=false) const;
  int getNumber(const string &name) const;
  
  Data d();
    // old school . Almost not needed.
    
private:
  DictO _data;
};

} // nodes

#endif // H_dynamicrow
