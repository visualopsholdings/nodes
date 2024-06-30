/*
  dynamicrow.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 30-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "storage/dynamicrow.hpp"

#include <boost/log/trivial.hpp>

string DynamicRow::getString(const string &name) {

  try {
    return boost::json::value_to<string>(_json.at(name));
  }
  catch (const boost::system::system_error& ex) {
    BOOST_LOG_TRIVIAL(error) << "field "<< name << " not found";
    return "";
  }

}

boost::json::array DynamicRow::getArray(const string &name) {

  try {
    return _json.at(name).as_array();
  }
  catch (const boost::system::system_error& ex) {
    BOOST_LOG_TRIVIAL(error) << "field "<< name << " not found";
    return {};
  }

}

vector<string> DynamicRow::getStringArray(const string &name) {

  vector<string> a;
  try {
    for (auto i: _json.at(name).as_array()) {
      a.push_back(i.as_string().c_str());
    }
  }
  catch (const boost::system::system_error& ex) {
    BOOST_LOG_TRIVIAL(error) << "field "<< name << " not found";
  }

  return a;
  
}
