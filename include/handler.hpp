/*
  handler.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Oct-2024
    
  Top level code for adding, updating, deleting and upstreaming objects
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_handler
#define H_handler

#include <string>
#include <boost/json.hpp>

using namespace std;
using json = boost::json::value;

namespace nodes {

class Server;

class Handler {

public:

  static bool add(Server *server, const string &type, const json &obj, optional<string> corr);
    // add a new object.
    
  static bool update(Server *server, const string &type, const string &id, 
                optional<string> me, optional<string> name, const json &obj);
    // update an existing object.
    
  static bool remove(Server *server, const string &type, const string &id, optional<string> me);
    // remove (delete) an existing object.
    
  static bool upstream(Server *server, const string &type, const string &id, const string &namefield);
    // get an object from the upstream server.
    
};

} // nodes

#endif // H_handler
