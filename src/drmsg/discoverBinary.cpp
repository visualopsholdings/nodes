/*
  discoverBinary.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 1-Dec-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "log.hpp"

#include <filesystem>

namespace fs = std::filesystem;

const auto ONEM = 10 * 1024 * 1024;
const auto MAX_FILE_SIZE = 4 * ONEM;
const auto MAX_TOTAL_SIZE = 10 * ONEM;

namespace nodes {

void discoverBinaryMsg(Server *server, const IncomingMsg &in) {
   
  string src;
  if (!server->getSrc(in, &src)) {
    server->sendErrDown("discoverBinary missing src");
    return;
  }

  L_INFO("<- discoverBinary " << src);
  
  // we will send 10K of chunks down. Work out how many files we could send 
  // for that.
  vector<tuple<string, string> > files;
  int tsize = 0;
  bool finished = true;
  for (auto i: in.extra_fields) {
  
    auto coll = get<0>(i);
    auto v = Dict::getVector(get<1>(i));
    if (!v) {
      server->sendErrDown("discoverBinary missing vector objects for " + coll);
      return;
    }
    for (auto i: *v) {
      auto id = Dict::getStringG(i, "id");
      if (!id) {
        server->sendErrDown("discoverBinary missing id field for " + Dict::toString(i));
        return;
      }
      auto uuid = Dict::getStringG(i, "uuid");
      if (!uuid) {
        server->sendErrDown("discoverBinary missing uuid field for " + Dict::toString(i));
        return;
      }
      // how big is the binary file.
      stringstream ss;
      ss << server->_mediaDir;
      ss << "/";
      ss << *uuid;
      int size;
      try {
        size = fs::file_size(ss.str());
      }
      catch (exception &exc) {
        L_ERROR("couldn't get file size " << ss.str());
        continue;
      }
      if (size > MAX_FILE_SIZE) {
        L_WARNING("skipping file " << ss.str() << " too large " << size);
        continue;
      }
      files.push_back(tuple<string, string>(*id, ss.str()));
      tsize += size;
      if (tsize > MAX_TOTAL_SIZE) {
        L_WARNING("that's too many enough files " << tsize);
        finished = false;
        break;
      }
    }
  }
  
  L_INFO("sending files " << files.size());
  
  // ok now we have our files, we split all of them into parts and send them.
  DictO msg = dictO({
    { "type", "discoverBinaryResult" }
  });
  
  // and send the result on.
  server->sendDown(msg);

}

};
