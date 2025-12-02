/*
  discoverBinaryResult.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 1-Dec-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"

#include "storage.hpp"
#include "security.hpp"
#include "log.hpp"
#include "bin.hpp"

namespace nodes {

void discoverBinaryResultMsg(Server *server, const char *data, size_t size) {
   
  string type, id, uuid;
  Bin::fileMsgDetails(data, size, &type, &id, &uuid);

  // test here if we should go again!
  stringstream ss;
  ss << server->_mediaDir;
  ss << "/";
  ss << uuid;
  
  L_DEBUG("writing to file " << ss.str());
  
  size = Bin::writeFileMsg(ss.str(), data, size);
  
  if (Bin::finishedFile(data, size)) {
    // write the status as 1
    // go and get the next file.
    L_DEBUG("set status for " << type << " " << id);
    return;
  }
  
  // ask for the next bit of the file.
  L_DEBUG("ask for more of " << type << " " << id);

}

};