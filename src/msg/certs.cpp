/*
  certs.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 11-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"

#include <boost/log/trivial.hpp>

void Server::certsMsg(json &j) {

  if (_certFile.empty()) {
    send({
      { "type", "certs" }, 
      { "ssl", false }
    });
  }
  else {
    send({
      { "type", "certs" }, 
      { "ssl", true },
      { "certFile", _certFile }, 
      { "chainFile", _chainFile }
    });
  }

}
