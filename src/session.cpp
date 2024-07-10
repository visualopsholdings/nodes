/*
  session.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 9-Jul-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "session.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

using namespace std;

shared_ptr<Sessions> Sessions::_instance;

string Sessions::create(const string &userid, bool admin) {

  auto uuid = boost::uuids::to_string(boost::uuids::random_generator()());
  
  _sessions[uuid] = shared_ptr<Session>(new Session(userid, admin));
  
  return uuid;
  
}
