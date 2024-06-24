/*
  b64.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 24-Jun-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "b64.hpp"

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/transform_width.hpp>

using namespace boost::archive::iterators;

typedef transform_width<binary_from_base64<string::const_iterator> , 8, 6 > binary_t;

string B64::toBinary(const string &s) {

  return string(binary_t(s.begin()), binary_t(s.end()));

}
