/*
  data.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 27-Dec-2024
    
  Data for nodes. It's a wrapper around boost::json for now.
  
  construct using an initialiser list like this:
  
  Data d = {
   { "str", "a string" }
  };
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_data
#define H_data

#include <boost/json.hpp>

using namespace std;

namespace nodes {

class Security;

class Data: public boost::json::value {
//class Data {
 
public:

  Data() : boost::json::value() {}
  Data(initializer_list<boost::json::value_ref> init) : boost::json::value(init) {}
//  Data(initializer_list<boost::json::value_ref> init) {}
  Data(const string &s);
    
  bool has(const string &name);
    // does it have the key.
    
  // getters.
  optional<string> getString(const string &name, bool silent=false);
  optional<long> getNumber(const string &name, bool silent=false);
  optional<bool> getBool(const string &name, bool silent=false);
  optional<Data> getArray(const string &name, bool silent=false);

  // setters.
  void setString(const string &name, const string &val);
  void setBool(const string &name, bool val);
  void setObj(const string &name, const Data &val);
  
private:
  friend class Security;
  
  Data(boost::json::value &init) :
    boost::json::value(init) {}
  Data(boost::json::array &init) :
    boost::json::value(init) {}
  Data(boost::json::object &init) :
    boost::json::value(init) {}

//   Data(boost::json::value &init) {}
//   Data(boost::json::array &init) {}
//   Data(boost::json::object &init) {}
    
};

} // nodes

#endif // H_data
