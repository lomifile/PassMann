//
// Created by filip on 17. 02. 2021..
//

#ifndef PASSMANN_USER_H
#define PASSMANN_USER_H

#define NAME_MAX_CHAR 150
#define USERNAME_MAX_CHAR 255
#define PASSWORD_MAX_CHAR 255
#define MAX_USER 2

typedef struct {
    char name[NAME_MAX_CHAR];
    char username[USERNAME_MAX_CHAR];
    char password[PASSWORD_MAX_CHAR];
} Userdata;

typedef struct {
    Userdata user[MAX_USER];
    int row;
} User;

extern Userdata userdata;
extern User user;

void insert(Userdata data, int pos, User *user);

void save(User *user);

void load(User *user);

void update(User *user);

void input_user(User *user);

void print_user_list(User *user);

int check_user_file();

#endif //PASSMANN_USER_H
