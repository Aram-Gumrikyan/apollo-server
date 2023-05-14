//
// Created by aram on 5/15/23.
//

#ifndef APOLLO_SERVER_AES_H
#define APOLLO_SERVER_AES_H

#include <openssl/evp.h>

namespace aes {
    int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
                unsigned char *iv, unsigned char *ciphertext) {
        EVP_CIPHER_CTX *ctx;

        int len;

        int ciphertext_len;

        ctx = EVP_CIPHER_CTX_new();

        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);

        EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);

        ciphertext_len = len;

        EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
        ciphertext_len += len;

        EVP_CIPHER_CTX_free(ctx);

        return ciphertext_len;
    }

    int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                unsigned char *iv, unsigned char *plaintext) {
        EVP_CIPHER_CTX *ctx;

        int len;

        int plaintext_len;

        ctx = EVP_CIPHER_CTX_new();

        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);

        EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
        plaintext_len = len;

        EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
        plaintext_len += len;

        EVP_CIPHER_CTX_free(ctx);

        return plaintext_len;
    }
}

#endif //APOLLO_SERVER_AES_H
