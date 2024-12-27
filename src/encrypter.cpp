/*
  encrypter.cpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 13-Sep-2024
    
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#include "encrypter.hpp"

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include "log.hpp"
#include <boost/algorithm/hex.hpp>

namespace nodes {

Encrypter::Encrypter(const string &key, const string &iv) {

  if (!(_ctx = EVP_CIPHER_CTX_new())) {
    L_ERROR("Could not create EVP_CIPHER_CTX");
  }
  
  _key = boost::algorithm::unhex(key);
  _iv = boost::algorithm::unhex(iv);
}

Encrypter::~Encrypter() {
  EVP_CIPHER_CTX_free(_ctx);
  _ctx = 0;
}

optional<string> Encrypter::encryptText(const string &data) {

  if (!_ctx) {
    L_ERROR("Initialise failed.");
    return nullopt;
  }
  
  if (EVP_EncryptInit_ex(_ctx, EVP_aes_256_cbc(), NULL, (unsigned char *)_key.c_str(), (unsigned char *)_iv.c_str()) != 1) {
    L_ERROR("Could not setup encryption");
    return nullopt;
  }
  
  unsigned char ciphertext[256];
  int len;
  if (EVP_EncryptUpdate(_ctx, ciphertext, &len, (unsigned char *)data.c_str(), data.size()) != 1) {
    L_ERROR("Could not encrypt");
    return nullopt;
  }
  int ciphertext_len = len;
  
  if (EVP_EncryptFinal_ex(_ctx, ciphertext + len, &len) != 1) {
    L_ERROR("Could not finalize encrypt");
    return nullopt;
  }
  ciphertext_len += len;
  
  return boost::algorithm::hex(string((const char *)ciphertext, ciphertext_len)); 

}

optional<string> Encrypter::decryptText(const string &data) {

  if (!_ctx) {
    L_ERROR("Initialise failed.");
    return nullopt;
  }
  string ciphertext = boost::algorithm::unhex(data); 

  if (EVP_DecryptInit_ex(_ctx, EVP_aes_256_cbc(), NULL, (unsigned char *)_key.c_str(), (unsigned char *)_iv.c_str()) != 1) {
    L_ERROR("Could not setup decryption");
    return nullopt;
  }
  
  unsigned char plaintext[256];
  int len;
  if (EVP_DecryptUpdate(_ctx, plaintext, &len, (unsigned char *)ciphertext.c_str(), ciphertext.size()) != 1) {
    L_ERROR("Could not decrypt");
    return nullopt;
  }
  int plaintext_len = len;

  int result = EVP_DecryptFinal_ex(_ctx, plaintext + len, &len);
  if (result != 1) {
    L_ERROR("Could not finalize decrypt " << result);
    return nullopt;
  }
  plaintext_len += len;
  
  return string((const char *)plaintext, plaintext_len);

}

string Encrypter::makeKey() {
  unsigned char key[64];
  RAND_bytes(key, sizeof(key));
  return boost::algorithm::hex(string((const char *)key, sizeof(key)));
}

string Encrypter::makeIV() {
  unsigned char iv[12];
  RAND_bytes(iv, sizeof(iv));
  return boost::algorithm::hex(string((const char *)iv, sizeof(iv)));
}

} // nodes