/*
  b64.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 24-Jun-2024
    
  Base64 code for ZMQChat
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_b64
#define H_b64

#include <string>

using namespace std;

class B64 {

public:

  static string toBinary(const string &s);

};

#endif // H_b64
