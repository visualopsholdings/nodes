/*
  certs.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 11-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"

#include "log.hpp"

namespace nodes {

void certsMsg(Server *server, json &j) {

  if (server->_certFile.empty()) {
    server->send({
      { "type", "certs" }, 
      { "ssl", false }
    });
  }
  else {
    server->send({
      { "type", "certs" }, 
      { "ssl", true },
      { "certFile", server->_certFile }, 
      { "chainFile", server->_chainFile }
    });
  }

}

};