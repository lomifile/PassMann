/**
 * Hashing
 * 
 * Contains function that hashes a string, mostly used for passwords
 * 
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

#include "hashing.h"

char *getHash(char *string) {
    unsigned char *hashed = malloc(SHA256_DIGEST_LENGTH);
    char *hstring = (char *) malloc(sizeof(hashed));
    SHA256(string, strlen(string), hashed);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        char *temp = malloc(sizeof(hashed[i]));
        sprintf(temp, "%x", hashed[i]);
        strcat(hstring, temp);
    }

    return hstring;
}