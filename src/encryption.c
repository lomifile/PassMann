/**
 * Encryption
 * 
 * Contains 2 function for encrypting and decrypting data
 * 
 **/

#include <openssl/aes.h>
#include "encryption.h"

AES_KEY enc_key, dec_key;

char *encrypt_data(char *password) {
    AES_set_encrypt_key(key, 128, &enc_key);
    AES_encrypt(password, enc_out, &enc_key);
    return enc_out;
}

char *decrypt_data(char *password) {
    AES_set_decrypt_key(key, 128, &dec_key);
    AES_decrypt(password, dec_out, &dec_key);
    return dec_out;
}