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
#include <boost/log/trivial.hpp>
#include <boost/algorithm/hex.hpp>

#define APP_SALT "xh+96tl44/mhX0FvV2zwcITgAcc9+4NwANHIg2LnY6oHnW7EqDxbXGZkYxy+Bb2R6scglcG3JqaBZTjBfVH5EkVpsKzKoXS4leIvmVaqC551RPgBNStn5skSUipq/JvG/J4JUlulj+T+KqKPQF25jhYpcbnLqQhRu3Spy18z7T0="
#define APP_IV ""

Encrypter::Encrypter() {

  if (!(_ctx = EVP_CIPHER_CTX_new())) {
    BOOST_LOG_TRIVIAL(error) << "Could not create EVP_CIPHER_CTX";
  }
  
}

Encrypter::~Encrypter() {
  EVP_CIPHER_CTX_free(_ctx);
  _ctx = 0;
}

optional<string> Encrypter::encryptText(const string &data) {

  if (!_ctx) {
    BOOST_LOG_TRIVIAL(error) << "Initialise failed.";
    return nullopt;
  }
  
  if (EVP_EncryptInit_ex(_ctx, EVP_aes_256_cbc(), NULL, (unsigned char *)APP_SALT, (unsigned char *)APP_IV) != 1) {
    BOOST_LOG_TRIVIAL(error) << "Could not setup encryption";
    return nullopt;
  }
  
  unsigned char ciphertext[256];
  int len;
  if (EVP_EncryptUpdate(_ctx, ciphertext, &len, (unsigned char *)data.c_str(), data.size()) != 1) {
    BOOST_LOG_TRIVIAL(error) << "Could not encrypt";
    return nullopt;
  }
  int ciphertext_len = len;
  
  if (EVP_EncryptFinal_ex(_ctx, ciphertext + len, &len) != 1) {
    BOOST_LOG_TRIVIAL(error) << "Could not finalize encrypt";
    return nullopt;
  }
  ciphertext_len += len;
  
  return boost::algorithm::hex(string((const char *)ciphertext, ciphertext_len)); 

}

optional<string> Encrypter::decryptText(const string &data) {

  if (!_ctx) {
    BOOST_LOG_TRIVIAL(error) << "Initialise failed.";
    return nullopt;
  }
  string ciphertext = boost::algorithm::unhex(data); 

  if (EVP_DecryptInit_ex(_ctx, EVP_aes_256_cbc(), NULL, (unsigned char *)APP_SALT, (unsigned char *)APP_IV) != 1) {
    BOOST_LOG_TRIVIAL(error) << "Could not setup decryption";
    return nullopt;
  }
  
  unsigned char plaintext[256];
  int len;
  if (EVP_DecryptUpdate(_ctx, plaintext, &len, (unsigned char *)ciphertext.c_str(), ciphertext.size()) != 1) {
    BOOST_LOG_TRIVIAL(error) << "Could not decrypt";
    return nullopt;
  }
  int plaintext_len = len;

  int result = EVP_DecryptFinal_ex(_ctx, plaintext + len, &len);
  if (result != 1) {
    BOOST_LOG_TRIVIAL(error) << "Could not finalize decrypt " << result;
    return nullopt;
  }
  plaintext_len += len;
  
  return string((const char *)plaintext, plaintext_len);

}
