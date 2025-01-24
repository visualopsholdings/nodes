/*
  data.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 27-Dec-2024
    
  Data for nodes. It's a wrapper around boost::json for now.
  
  construct using an initialiser list like this:
  
  Data d = {
   { "str", "a string" }
  };
  
  It's a little different to boost::json::value in that if you think it's an array
  you can just use it like that, and you can also access it just like an object rather
  than casting it so.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_data
#define H_data

#include <boost/json.hpp>

using namespace std;

namespace nodes {

class Security;
template <typename RowType>
class Result;
class GroupRow;
class UserRow;
class IndexRow;
class DynamicRow;
class SiteRow;
class InfoRow;
class NodeRow;
class PolicyRow;

class DataArray {
};

class Data: public boost::json::value {
 
public:

  Data() : boost::json::value() {}
  Data(const DataArray &) : boost::json::value(boost::json::array()) {}
  Data(boost::json::value &init) : boost::json::value(init) {}
  Data(initializer_list<boost::json::value_ref> init) : boost::json::value(init) {}
  Data(const string &s);
    
  bool has(const string &name);
    // does it have the key.
    
  // act like an object.
  optional<string> getString(const string &name, bool silent=false) const;
  optional<long> getNumber(const string &name, bool silent=false) const;
  optional<bool> getBool(const string &name, bool silent=false) const;
  optional<Data> getData(const string &name, bool silent=false);
  void setString(const string &name, const string &val);
  void setBool(const string &name, bool val);
  void setObj(const string &name, const Data &val);
  
  // act like an array.
  boost::json::array::iterator begin();
  boost::json::array::iterator end();
  boost::json::array::const_iterator begin() const;
  boost::json::array::const_iterator end() const;
  Data getIterator(boost::json::array::iterator);
  size_t size() const;
  void push_back(const Data &data);
  void pop_back();
   
private:

   friend class Security;
   friend class ResultImpl;
   friend class Storage;
   friend class Server;
  
  Data(boost::json::array &init) :
    boost::json::value(init) {}
  Data(boost::json::object &init) :
    boost::json::value(init) {}

};

} // nodes

#endif // H_data
