/*
  vid.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-June-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "vid.hpp"

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/hex.hpp>
#include <iostream>

using namespace boost::archive::iterators;

typedef transform_width<binary_from_base64<string::const_iterator> , 8, 6 > binary_t;
  
//  0123 	4 - 27 		28 29		30 - 
//. 564f 	UID 		00 | 01		token or password
//  "564f" == "VO" as hex :=)

VID::VID(const string &vid) {

  string dec(binary_t(vid.begin()), binary_t(vid.end()));

  _header = dec.substr(0, 2);
  string uuid =  dec.substr(2, 12);
  boost::algorithm::hex(uuid.begin(), uuid.end(), back_inserter(_uuid));
  _token = dec[12] == '\0';
  _password = dec.substr(14);

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