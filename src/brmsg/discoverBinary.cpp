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

const auto ONEM = 1024 * 1024;
const auto MAX_FILE_SIZE = 2 * ONEM;
//const auto CHUNK_SIZE = 2 * ONEM;

namespace nodes {

void discoverBinaryMsg(Server *server, const IncomingMsg &in) {
   
  L_INFO("<- discoverBinary");
  
  // make sure data is ALWAYS sent.
  SendBinData data(server);

  auto first = *in.extra_fields.begin();
  auto type = get<0>(first);
  auto obj = Dict::getObject(get<1>(first));
  
  if (!obj) {
    Bin::createFileErrMsg(data.data(), type, "??", "??", "discoverBinary missing object for " + type);
    return;
  }
  auto id = Dict::getString(*obj, "id");
  if (!id) {
    Bin::createFileErrMsg(data.data(), type, "??", "??", "discoverBinary missing id field for " + type);
    return;
  }
  auto uuid = Dict::getString(*obj, "uuid");
  if (!uuid) {
    Bin::createFileErrMsg(data.data(), type, *id, "??", "discoverBinary missing uuid field for " + type);
    return;
  }
  
  if (in.extra_fields.size() == 0) {
    Bin::createFileErrMsg(data.data(), type, *id, *uuid, "discoverBinary missing objects");
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
    L_ERROR(exc.what());
    Bin::createFileErrMsg(data.data(), type, *id, *uuid, "couldn't get file size");
    return;
  }
  
  if (size > MAX_FILE_SIZE) {
    L_WARNING("skipping file " << ss.str() << " too large " << size);
    Bin::createFileTooLargeMsg(data.data(), type, *id, *uuid, size);
    return;
  }
  
//   if (size > CHUNK_SIZE) {
//     server->sendErrDownBin("not supported yet");
//     return;
//   }
// 
  // send the file
  ifstream file(ss.str(), ios::binary);
  if (!file.is_open()) {
    Bin::createFileErrMsg(data.data(), type, *id, *uuid, "couldn't open file");
    return;
  }
  
  // send the file message down.
  Bin::createFileMsg(file, data.data(), type, *id, *uuid, 0, size);
  file.close();
  
}

};
