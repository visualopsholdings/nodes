/*
  discoverBinary.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 1-Dec-2025
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "server.hpp"
#include "log.hpp"
#include "bin.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

const auto ONEK = 1024;

namespace nodes {

void discoverBinaryMsg(Server *server, const IncomingMsg &in) {
   
  L_INFO("<- discoverBinary");
  
  L_TRACE("maxFileSize " << server->_maxFileSize);
  L_TRACE("chunkSize " << server->_chunkSize);
  
  // are we doing a full scan?
  auto fullscan = in.full ? *in.full : false;
  
  // what is the offset?
  auto offset = in.offset ? *in.offset : 0;

  // make sure data is ALWAYS sent.
  SendBinData data(server);

  if (in.extra_fields.size() == 0) {
    Bin::createFileErrMsg(data.data(), "??", "??", "??", fullscan, "discoverBinary missing objects");
    return;
  }

  auto first = *in.extra_fields.begin();
  auto type = get<0>(first);
  auto obj = Dict::getObject(get<1>(first));
  
  if (!obj) {
    Bin::createFileErrMsg(data.data(), type, "??", "??", fullscan, "discoverBinary missing object for " + type);
    return;
  }
  auto id = Dict::getString(*obj, "id");
  if (!id) {
    Bin::createFileErrMsg(data.data(), type, "??", "??", fullscan, "discoverBinary missing id field for " + type);
    return;
  }
  auto uuid = Dict::getString(*obj, "uuid");
  if (!uuid) {
    Bin::createFileErrMsg(data.data(), type, *id, "??", fullscan, "discoverBinary missing uuid field for " + type);
    return;
  }
  
  
  std::filesystem::path p(server->_mediaDir);
  p /= *uuid;
   // how big is the binary file.
  int size;
  try {
    size = fs::file_size(p);
  }
  catch (exception &exc) {
    L_ERROR(exc.what());
    Bin::createFileErrMsg(data.data(), type, *id, *uuid, fullscan, "couldn't get file size");
    return;
  }
  
  if (size > (server->_maxFileSize * ONEK)) {
    L_WARNING("skipping file " << p.string() << " too large " << size);
    Bin::createFileTooLargeMsg(data.data(), type, *id, *uuid, fullscan, size);
    return;
  }
  
  // send the whole file
  ifstream file(p, ios::binary);
  if (!file.is_open()) {
    Bin::createFileErrMsg(data.data(), type, *id, *uuid, fullscan, "couldn't open file");
    return;
  }
  
  // send the message down, but in chunk sizes.
  Bin::createFileMsg(file, data.data(), type, *id, *uuid, fullscan, offset, (server->_chunkSize * ONEK));
  file.close();
  
}

};
