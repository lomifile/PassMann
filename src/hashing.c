//
// Created by filip on 17. 02. 2021..
//

#include "hashing.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

char* getHash(unsigned char* string){
    unsigned char* hashed = malloc(SHA256_DIGEST_LENGTH);
    unsigned char* hstring = (char*)malloc(sizeof(hashed));
    SHA256(string, strlen(string), hashed);

    for(int i =0; i < SHA256_DIGEST_LENGTH; i++){
        char* temp = malloc(sizeof(hashed[i]));
        sprintf(temp,"%x", hashed[i]);
        strcat(hstring, temp);
    }

    return hstring;
}