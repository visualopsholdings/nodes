/*
  encrypter.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 13-Sep-2024
    
  Code for encrypting and decrypting
  
  Licensed under [version 3 of the GNU General Public License] contained in LICENSE.
 
  https://github.com/visualopsholdings/nodes
*/

#ifndef H_encrypter
#define H_encrypter

#include <string>
#include <optional>

typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;

using namespace std;

class Encrypter {

public:
  Encrypter();
  ~Encrypter();
  
  optional<string> encryptText(const string &s);
  optional<string> decryptText(const string &s);

private:
  EVP_CIPHER_CTX *_ctx;

};

#endif // H_encrypter
