/*
  security.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 23-Jun-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/zmqchat
*/

#include "security.hpp"

#include "vid.hpp"
#include "b64.hpp"

#include <openssl/evp.h>

#define SHA1_LEN    128
#define ITERATIONS  12000

bool Security::valid(const VID &vid, const string &salt, const string &hash) {

  string password = vid.password();
  
  unsigned char out[SHA1_LEN+1];
  int len = PKCS5_PBKDF2_HMAC_SHA1((const char *)password.c_str(), password.length(), (const unsigned char *)salt.c_str(), salt.length(), ITERATIONS, SHA1_LEN, out);
  
  string dechash = B64::toBinary(hash);
  
  for (int i=0; i<SHA1_LEN; i++) {
    if ((unsigned char)dechash[i] != out[i]) {
      return false;
    }
  }
  
  return true;
  
}