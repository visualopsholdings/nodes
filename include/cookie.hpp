/*
  cookie.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 24-Jun-2024
    
  Cookie code for ZMQChat
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_b64
#define H_b64

#include <optional>
#include <string>

using namespace std;

class Cookie {

public:

  static optional<string> parseCookie(const string &cookie);
    // parse our session cookie out.

};

#endif // H_b64
