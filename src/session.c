//
// Created by filip on 17. 02. 2021..
//

#include <stdio.h>
#include "session.h"
#include "hashing.h"
#include <string.h>
#include <stdlib.h>

char username[USERNAME_MAX_CHAR];
char password[PASSWORD_MAX_CHAR];

void strip_null(char* buf){
    int len;
    buf[sizeof(buf) - 1] = '\0';
/* compute the length, and truncate the \n if any */
    len = strlen(buf);
    while ( len > 0 && buf[len - 1] == '\n' )
    {
        buf[len - 1] = '\0';
        --len;
    }
}

char* input_username(){
    printf("Input username> ");
    scanf(" %s", &username);
    return username;
}

char* input_password(){
    printf("Input password> ");
    system("stty -echo");
    scanf(" %s", &password);
//    char* hashed = getHash(password);
    system("stty echo");
    printf("\n");
    return password;
}

void insert_session(User* user, int pos, Session* session, char* username, char* password){
    for(int index = 0; index <= user->row; index++){
        if(strcmp(username, user->user[index].username) == 0 && strcmp(password, user->user[index].password) == 0){
            int position;
            if(session->last >= MAX_USER_SESSION)
                printf("User is already in session!");
            else if((position > session->last+1) || (position<0))
                printf("Position is full");
            else{
                for(position = session->last; position>=pos; position--)
                    session->user[position+1] = session->user[position];
                session->last++;
                session->user[pos] = user->user[index];
            }
        }
    }
}

int authentication(User* user, char* username, char* password){
    Session session;
    session.last = -1;
    int num = 0;
    do{
        int result = search(user, username, password);
        if(result == 0){
            printf("Wrong input!! \n");
            username = input_username();
            password = input_password();
            num++;
            continue;
        } else if(result == 1) {
            printf("Hello %s \n", username);
            insert_session(user, session.last+1, user, username, password);
            return 1;
        } else if(num == 3){
            printf("Too many tries try again!");
            break;
        }
    }while(1);
    return 0;
}

int search(User* user, char* username, char* password){
    //username search
    for(int index = 0; index <= user->row; index++){
        if(memcmp(username, user->user[index].username, strlen(username)) == 0 && memcmp(password, user->user[index].password, strlen(password)) == 0){
            return 1;
        }
    }
    return 0;
}