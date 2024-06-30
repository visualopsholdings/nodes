/*
  dynamicrow.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 30-June-2024
    
  A row that contains JSON for ZMQChat.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
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

protected:
  string getString(const string &name);
  vector<string> getStringArray(const string &name);
  boost::json::array getArray(const string &name);
  
private:
  json _json;
};

#endif // H_dynamicrow
