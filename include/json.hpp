/*
  json.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 2-Jul-2024
    
  JSON code for Nodes
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_json
#define H_json

#include <string>
#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

class Json {

public:

  static string toISODate(json &date);
  static bool has(const json &j, const string &name);
  static optional<string> getString(const json &j, const string &name, bool silent=false);
  static optional<boost::json::array> getArray(json &reply, const string &name, bool silent=false);
  static optional<json> getObject(const json &j, const string &name, bool silent=false);
  static optional<bool> getBool(const json &j, const string &name, bool silent=false);
  static optional<long> getNumber(const json &j, const string &name, bool silent=false);
  
  static bool appendArray(boost::json::object *obj, const string &name, const string &val);
    // given the object and the name of a member that contains an array, find the val
    // in the array. If it's found then false is returned. otherwise the value
    // is appended to the array.
    
  static bool arrayHas(const boost::json::object &obj, const string &name, const string &val);
    // Return true if the array has the value
    
  static bool arrayTail(const boost::json::object &obj, const string &name, string *val);
    // return the value of the tail of the array. If for some reason we can't do this,
    // we return false.
    
private:

  static json getMember(const json &j, const string &name, bool silent);
  static bool hasArrayMember(const boost::json::object &obj, const string &name);
  static bool hasArrayValue(const boost::json::array &arr, const string &val);
 
};

#endif // H_json
