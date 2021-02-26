//
// Created by filip on 17. 02. 2021..
//

#ifndef PASSMANN_SESSION_H
#define PASSMANN_SESSION_H

#include "user.h"

#define MAX_USER_SESSION 1
typedef struct{
    Userdata user[MAX_USER_SESSION];
    int last;
}Session;

extern Userdata userdata;
extern User user;

void strip_newline(char* s);
char* input_username();
char* input_password();
void insert_session(User* user, int pos, Session* session, char* username, char* password);
int authentication(User* user, char* username,char* password);
int search(User* user, char* username,char* password);
void show_user_session(Session* session);

#endif //PASSMANN_SESSION_H
