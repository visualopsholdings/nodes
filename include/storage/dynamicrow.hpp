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

#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

class DynamicRow {

public:
  DynamicRow(json j): _json(j) {}
  
  json j() { return _json; }
  string id() { return getString("id"); }

protected:
  string getString(const string &name) const;
  vector<string> getStringArray(const string &name);
  boost::json::array getArray(const string &name);
  bool getBool(const string &name) const;
  int getNumber(const string &name) const;
  
private:
  json _json;
};

#endif // H_dynamicrow
