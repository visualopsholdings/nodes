/*
  certs.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 11-Jun-2024
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "server.hpp"

#include "storage.hpp"

#include <boost/log/trivial.hpp>

void Server::certsMsg(json &j, shared_ptr<Storage> storage) {

// TBD: source this from the server info
  send({
    { "type", "certs" }, 
//     { "ssl", false }
    { "ssl", true },
    { "certFile", "/etc/letsencrypt/live/irc.visualops.com/privkey.pem" }, 
    { "chainFile", "/etc/letsencrypt/live/irc.visualops.com/fullchain.pem" }
  });

}
