//
// Created by filip on 19. 02. 2021..
//

#ifndef PASSMANN_ENCRYPTION_H
#define PASSMANN_ENCRYPTION_H
#include <sodium.h>
#include <stdio.h>
#include <string.h>

#define CHUNK_SIZE 4096

int encrypt(const char *target_file, const char *source_file,
            const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);

int decrypt(const char *target_file, const char *source_file,
            const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]);

int check_db_file(char *filename);

#endif //PASSMANN_ENCRYPTION_H
