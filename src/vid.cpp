/*
  vid.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "vid.hpp"

#include "b64.hpp"

#include <boost/algorithm/hex.hpp>
#include <boost/archive/iterators/dataflow_exception.hpp>
#include <iostream>

//  0123 	4 - 27 		28 29		30 - 
//. 564f 	UID 		00 | 01		token or password
//  "564f" == "VO" as hex :=)

VID::VID(const string &vid) {

  string dec;
  
  try {
    dec = B64::toBinary(vid);
  } 
  catch (boost::archive::iterators::dataflow_exception &ex) {
    // not base64, don't try to parse.
    return;
  }
  
  if (dec.length() < 16) {
    return;
  }
  
  _header = dec.substr(0, 2);
  string uuid =  dec.substr(2, 12);
  boost::algorithm::hex(uuid.begin(), uuid.end(), back_inserter(_uuid));
  _token = dec[12] == '\0';
  _password = dec.substr(15);

}

bool VID::valid() {
  return _header == "VO";
}

void VID::describe() {

  cout << "header: " << _header << endl;
  cout << "uuid: " << _uuid << endl;
  cout << "token?: " << (_token ? "Y" : "N") << endl;
  cout << "password: " << _password << endl;

}