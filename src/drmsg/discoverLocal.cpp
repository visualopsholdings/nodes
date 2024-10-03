/*
  discoverLocal.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 3-Oct-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "json.hpp"
#include "storage/schema.hpp"

#include <boost/log/trivial.hpp>

namespace nodes {

void discoverLocalMsg(Server *server, json &j) {
   
  BOOST_LOG_TRIVIAL(trace) << "discoverLocal " << j;
 
  auto data = Json::getArray(j, "data");
  if (!data) {
    server->sendErrDown("discoverLocal missing data");
    return;
  }
  
  if (data.value().size() > 0) {
    // here is where you would import all the things that this
    // node had discovered locally that had changed.
    BOOST_LOG_TRIVIAL(warning) << "discoverLocal data not implemented";
  }  
 	    
 	server->sendDown({ 
 	  { "type", "discoverLocalResult" } 
 	});
 	
}

};
