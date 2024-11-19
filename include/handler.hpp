/*
  handler.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 21-Oct-2024
    
  Top level code for adding, updating and deleting objects
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_handler
#define H_handler

#include "storage/schema.hpp"

#include <string>

using namespace std;

class Server;

template <typename RowType>
class Handler {

public:

  static bool add(Server *server, Schema<RowType> &schema, const string &type, const string &me, const string &name);
    // add a new object.
    // used for stream and group
    
  static bool update(Server *server, Schema<RowType> &schema, const string &type, const string &id, 
                optional<string> me, optional<string> name, boost::json::object *obj);
    // update an existing object.
    // used for stream and group
    
  static bool remove(Server *server, Schema<RowType> &schema, const string &type, const string &id, 
                optional<string> me);
    // remove (delete) an existing object.
    // used for stream, group and user
    
  static bool upstream(Server *server, Schema<RowType> &schema, const string &type, const string &id, const string &namefield);
    // get an object from the upstream server.
    // used for stream, group and user
    
};

#endif // H_handler
