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
const auto MAX_FILE_SIZE = 4 * ONEM;
const auto CHUNK_SIZE = 2 * ONEM;

namespace nodes {

void discoverBinaryMsg(Server *server, const IncomingMsg &in) {
   
  L_INFO("<- discoverBinary ");
  
  if (in.extra_fields.size() == 0) {
    server->sendErrDownBin("discoverBinary missing objects");
    return;
  }
  
  auto first = *in.extra_fields.begin();
  auto type = get<0>(first);
  auto obj = Dict::getObject(get<1>(first));
  
  if (!obj) {
    server->sendErrDownBin("discoverBinary missing object for " + type);
    return;
  }
  auto id = Dict::getString(*obj, "id");
  if (!id) {
    server->sendErrDownBin("discoverBinary missing id field for " + type);
    return;
  }
  auto uuid = Dict::getString(*obj, "uuid");
  if (!uuid) {
    server->sendErrDownBin("discoverBinary missing uuid field for " + type);
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
    server->sendErrDownBin("couldn't get file size");
    return;
  }
  
  DictO msg = dictO({
    { "type", "discoverBinaryResult" }
  });

  if (size > MAX_FILE_SIZE) {
    L_WARNING("skipping file " << ss.str() << " too large " << size);
    msg[type] = dictO({
      { "id", *id },
      { "toolarge", true },
      { "size", size },      
    });
    server->sendDownBin(msg);
    return;
  }
  
  if (size > CHUNK_SIZE) {
    server->sendErrDownBin("not supported yet");
    return;
  }

  // send the file
  ifstream file(ss.str(), ios::binary);
  if (!file.is_open()) {
    server->sendErrDownBin("couldn't open file");
    return;
  }
  
  // message 1 is binary discover result.
  vector<char> data;
  Bin::createFileMsg(file, &data, 0, type, *id, *uuid, 0, size);
  file.close();
  
  // and send the result on.
  server->sendDownBin(data);

}

};
