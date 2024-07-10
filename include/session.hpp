/*
  session.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 9-Jul-2024
    
  Session management for ZMQChat
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#ifndef H_session
#define H_session

#include <string>
#include <map>
#include <memory>

using namespace std;

class Session {

public:
  Session(const string &userid, bool admin): _userid(userid), _admin(admin) {};
  
  bool isAdmin() { return _admin; };
  
private:
  string _userid;
  bool _admin;
  
};

class Sessions {

public:

  static shared_ptr<Sessions> instance() {
    if(!_instance) {
      _instance.reset(new Sessions());
    }
    return _instance;
  };
    
  bool has(const string &sessid) {
    return _sessions.find(sessid) != _sessions.end();
  }
  shared_ptr<Session> get(const string &sessid) {
    return _sessions[sessid];
  }
  string create(const string &userid, bool admin);
  
private:

  // there can be only 1.
  Sessions() {};
  static shared_ptr<Sessions> _instance;

  map<string, shared_ptr<Session> > _sessions;
  
};

#endif // H_session
