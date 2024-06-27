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
#include <boost/json.hpp>

using namespace std;

class VID;
class Storage;

class Security {

public:

  static bool valid(const VID &vid, const string &salt, const string &hash);
    // return true if the VID is valid. The salt is generated with the user, and the
    // hash is a premade hash of the password (we don't store the actual password)
    
  static void getPolicyUsers(Storage &storage, const string &id, vector<string> *users);
    // get a list of users that are in this policy.
  
private:

  static void addTo(vector<string> *v, const string &val);

};

#endif // H_security
