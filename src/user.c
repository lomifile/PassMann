/**
 * User 
 * 
 * This piece contains al the manipulations and data handling tools for 
 * creating user.
 * 
 */

#include "user.h"
#include "hashing.h"
#include "session.h"

void strip_nul(char* buf){
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

void flush_stdin()
{ int c; while ((c = getchar()) != '\n' && c != EOF); }

char *getUserName() {
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        return pw->pw_name;
    }

    return "";
}

void print_user_list(User *user) {
    printf("%10s|%10s|%10s|\n","Name","Username","Password");
    printf("=================================\n");
    for (int index = 0; index <= user->row; index++) {
        printf("%10s|%10s|%10s|\n", user->user[index].name, user->user[index].username, user->user[index].password);
    }
}

void input_user(User *user) {
    Userdata data;
    char password[PASSWORD_MAX_CHAR];
    fflush(stdin);
    printf("Input your name> ");
    fgets(data.name, NAME_MAX_CHAR + 1, stdin);
    fflush(stdin);
    strip_newline(data.name);
    int selection;
    do {
        printf("Do you want to use your PC username or you want a custom username? \nInput:\n(1) for PC\n(2) for custom\n");
        scanf("%d", &selection);
        if (selection == 1 || selection == 2) {
            break;
        }
        continue;
    } while (1);

    if (selection == 1) {
        strcpy(data.username, getUserName());
    } else if (selection == 2) {
        printf("Input your username> ");
        scanf("%s", data.username);
        strip_newline(data.username);
    }
    fflush(stdin);
    printf("Input password> ");
    system("stty -echo");
//    fgets(password, PASSWORD_MAX_CHAR + 1 , stdin);
    scanf("%s", password);
    strip_newline(password);
    system("stty echo");
    char* hashed = getHash(password);
    strcpy(data.password, hashed);

    insert(data, user->row + 1, user);
    flush_stdin();
}

void insert(Userdata data, int pos, User *user) {
    int position;
    if (user->row >= MAX_USER - 1)
        printf("There are too many users in!");

    else if ((pos > user->row + 1) || (pos < 0))
        printf("NULL position");
    else {
        for (position = user->row; position >= pos; position--)
            user->user[position + 1] = user->user[position];
        user->row++;
        user->user[pos] = data;
    }
}

void save(User *user) {
    FILE *file;
    file = fopen("User.dat", "wb");
    if (file == NULL)
        printf("Error reading user file");
    else {
        fwrite(user, sizeof(*user), 1, file);
    }
    fclose(file);
}

void load(User *user) {
    FILE *file;
    file = fopen("User.dat", "rb");
    if (file == NULL)
        printf("Error reading user file");
    else {
        fread(user, sizeof(*user), 1, file);
    }
    fclose(file);
}

int check_user_file() {
    if (access("User.dat", F_OK) == 0) {
        return 1;
    } else {
        return 0;
    }
}