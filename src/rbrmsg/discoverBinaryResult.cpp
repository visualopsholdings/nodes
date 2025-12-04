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

void markObj(const string &coll, const string &id, int status) {

  auto r = SchemaImpl::updateGeneralById(coll, id, dictO({
    { "$set", dictO({{ "binStatus", status }}) }
  }));
  if (!r) {
    L_ERROR("could not update " + coll + ":" + id);
  }
    
}

void discoverBinaryResultMsg(Server *server, const char *data, size_t size) {
   
  Bin binary(data, size);

  auto type = binary.getType();
  string coll;
  if (!Storage::instance()->collName(type, &coll)) {
    L_ERROR("Could not get collection name for " + type);
    return;
  }
  auto id = binary.getID();

  auto err = binary.getError();
  if (err) {
    L_ERROR(*err);
    markObj(coll, id, Bin::DOWNLOAD_ERR);
    server->discoverBinary();
    return;
  }
  
  if (binary.isTooLarge()) {
    markObj(coll, id, Bin::TOO_LARGE);
    server->discoverBinary();
    return;
  }
  
  // get the filenames.
  std::filesystem::path p(server->_mediaDir);
  p /= binary.getUUID();
  std::filesystem::path normp = p;
  if (binary.getOffset() > 0 || !binary.isFinished()) {
    p += "_";
  }
  
  L_TRACE("writing to file " << p.string());
  
  auto wsize = binary.writeFile(p.string());
  
  if (binary.isFinished()) {
    if (binary.getOffset() > 0) {
      // rename our file to not have an "_" at the end.
      std::filesystem::rename(p, normp);
    }
    markObj(coll, id, Bin::DOWNLOADED);
    server->discoverBinary();
    return;
  }
  
  DictO msg = dictO({
    { "type", "discoverBinary" },
    { "offset", (long)(binary.getOffset()+wsize) },
    { type, dictO({
      { "id", id },
      { "uuid", binary.getUUID() }
      })
    }
  });
  server->sendBinReq(msg);

}

};