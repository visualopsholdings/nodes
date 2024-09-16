/*
  vid.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Jun-2024
    
  VIDs for Nodes.
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_vid
#define H_vid

#include <string>

using namespace std;

class VID {

public:
  VID();
  VID(const string &vid);
  
  void reset(const string &uuid, const string &password);
  bool valid();
  string uuid() const { return _uuid; }
  string password() const { return _password; }
  void describe();
  string value() const;
  
  bool operator == (const VID &vid);
  
private:
  string _header;
  string _uuid;
  string _password;
    
};

#endif // H_vid
