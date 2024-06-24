/*
  security.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Jun-2024
    
  Security code for ZMQChat
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_security
#define H_security

#include <string>

using namespace std;

class VID;

class Security {

public:

  static bool valid(const VID &vid, const string &salt, const string &hash);

};

#endif // H_security
