#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "commands.c"
#include "../../headers/defines.h"
#include "../../headers/structs.h"
#include <sqlite3.h>

/* ------------------------------------------------------------------ */

const char* tokensToMessage(char** tokens, int startToken, int* tokenCount)
{
    char* message = malloc(1);
    int messageSize = 1;
    message[0] = '\0';

    for (int i = startToken; i <= *tokenCount; ++i)
    {
        messageSize += strlen(tokens[i]) + 1;
        message = realloc(message, messageSize);
        strcat(message, tokens[i]);
        strcat(message, " ");
    }

    message = realloc(message, messageSize - 1); //scoatem ultimul ' '
    message[messageSize - 2] = '\0';

    return message;
}

void freeTokens(char** tokens, int count)
{
    if (tokens == NULL)
        return;
    for (int i = 0; i < count; ++i)
        if (tokens[i] != NULL)
            free(tokens[i]);
    free(tokens);
}

void handleError(const char* error)
{
    char* output = malloc(32 + strlen(error));
    sprintf(output, "[server] Eroare la %s", error);
    perror(output);
    free(output);
    exit(EXIT_FAILURE);
}
void handleThreadError(const char* error)
{
    char* output = malloc(32 + strlen(error));
    sprintf(output, "[server] Eroare la %s", error);
    perror(output);
    free(output);
}

void initDatabase()
{
    int rc = sqlite3_open("virtualsoc.db", &db);
    if (rc)
        handleError("sqlite3_open()");

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, 0);
    
    const char* initTables =
        "BEGIN TRANSACTION;"
            // 1. Users
            "CREATE TABLE IF NOT EXISTS users ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "username TEXT NOT NULL UNIQUE,"
            "password TEXT NOT NULL,"
            "role INTEGER DEFAULT 0,"
            "privacy INTEGER DEFAULT 0,"
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");"

            // 2. Friends
            "CREATE TABLE IF NOT EXISTS friends ("
            "user_id INTEGER NOT NULL,"
            "friend_id INTEGER NOT NULL,"
            "type INTEGER DEFAULT 0,"
            "status INTEGER DEFAULT 0,"
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "PRIMARY KEY (user_id, friend_id),"
            "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE,"
            "FOREIGN KEY (friend_id) REFERENCES users(id) ON DELETE CASCADE"
            ");"

            // 3. Posts
            "CREATE TABLE IF NOT EXISTS posts ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user_id INTEGER NOT NULL,"
            "content TEXT NOT NULL,"
            "visibility INTEGER DEFAULT 0,"
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
            ");"

            // 4. Messages (Private)
            "CREATE TABLE IF NOT EXISTS messages ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "sender_id INTEGER NOT NULL,"
            "receiver_id INTEGER NOT NULL,"
            "message TEXT NOT NULL,"
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "FOREIGN KEY (sender_id) REFERENCES users(id) ON DELETE CASCADE,"
            "FOREIGN KEY (receiver_id) REFERENCES users(id) ON DELETE CASCADE"
            ");"

            // 5. Groups
            "CREATE TABLE IF NOT EXISTS groups ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL UNIQUE,"
            "owner_id INTEGER NOT NULL,"
            "FOREIGN KEY (owner_id) REFERENCES users(id)"
            ");"

            // 6. Group Members
            "CREATE TABLE IF NOT EXISTS group_members ("
            "group_id INTEGER NOT NULL,"
            "user_id INTEGER NOT NULL,"
            "PRIMARY KEY (group_id, user_id),"
            "FOREIGN KEY (group_id) REFERENCES groups(id) ON DELETE CASCADE,"
            "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
            ");"

            // 7. Group Messages
            "CREATE TABLE IF NOT EXISTS group_messages ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "group_id INTEGER NOT NULL,"
            "sender_id INTEGER NOT NULL,"
            "message TEXT NOT NULL,"
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "FOREIGN KEY (group_id) REFERENCES groups(id) ON DELETE CASCADE,"
            "FOREIGN KEY (sender_id) REFERENCES users(id) ON DELETE CASCADE"
            ");"

            "COMMIT;";

    char* zErrMsg = 0;
    rc = sqlite3_exec(db, initTables, 0, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare la sqlite3_exec(): %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        exit(EXIT_FAILURE);
    }
    else
        printf("[server] Am initializat baza de date!\n");
}

char** parseCommand(char* command, int* tokenCount)
{
    *tokenCount = 0;
    char** tokens = malloc(sizeof(char*));
    char* save_ptr = command;
    char* token = strtok_r(save_ptr, " ", &save_ptr);

    if (token == NULL)
        return NULL;
    
    int length = strlen(token);
    tokens[*tokenCount] = malloc(length + 1);
    strcpy(tokens[(*tokenCount)++], token);

    while ((token = strtok_r(NULL, " ", &save_ptr)) != NULL)
    {
        length = strlen(token);
        tokens = realloc(tokens, sizeof(char*) * (*tokenCount + 1));
        tokens[*tokenCount] = malloc(length + 1);
        strcpy(tokens[(*tokenCount)++], token);
    }

    return tokens;
}

char* handleCommand(char** tokens, int* tokenCount, int* client_id)
{
    for (int i = 0; i < *tokenCount; ++i)
        printf("[server] Token %d: %s\n", i, tokens[i]);
    if (*tokenCount == 0)
        RET_DYN(EMPTY_INPUT);
    char* bufferOut;
    const char* command = tokens[0];
    (*tokenCount)--;

    if (strcmp(command, REGISTER_COMMAND) == 0)
    {
        if (*tokenCount == REGISTER_ARGC)
        {
            const char* username = tokens[1];
            const char* password = tokens[2];
            const char* type = tokens[3];
            return registerCommand(username, password, type);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < REGISTER_ARGC)
            RET_DYN(LESS_ARGUMENTS);
        else if (*tokenCount > REGISTER_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, LOGIN_COMMAND) == 0)
    {
        if (*tokenCount == LOGIN_ARGC)
        {
            //printf("[login_server] client_id: %d\n", *client_id);
            const char* username = tokens[1];
            const char* password = tokens[2];
            return loginCommand(username, password, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < LOGIN_ARGC)
            RET_DYN(LESS_ARGUMENTS);
        else if (*tokenCount > LOGIN_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, LOGOUT_COMMAND) == 0)
    {
        if (*tokenCount == LOGOUT_ARGC)
            return logoutCommand(client_id);
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < LOGOUT_ARGC)
            RET_DYN(LESS_ARGUMENTS);
        else if (*tokenCount > LOGOUT_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, ADD_COMMAND) == 0)
    {
        if (*tokenCount == ADD_ARGC)
        {
            const char* username = tokens[1];
            return addCommand(username, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < ADD_ARGC)
            RET_DYN(LESS_ARGUMENTS);
        else if (*tokenCount > ADD_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, ACCEPT_COMMAND) == 0)
    {
        if (*tokenCount == REQUEST_ARGC)
        {
            const char* username = tokens[1];
            return acceptCommand(username, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < REQUEST_ARGC)
            RET_DYN(LESS_ARGUMENTS);
    }
    else if (strcmp(command, DECLINE_COMMAND) == 0)
    {
        if (*tokenCount == REQUEST_ARGC)
        {
            const char* username = tokens[1];
            return declineCommand(username, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < REQUEST_ARGC)
            RET_DYN(LESS_ARGUMENTS);
    }
    else if (strcmp(command, REQUESTS_COMMAND) == 0)
    {
        if (*tokenCount == REQUESTS_ARGC)
            return requestsCommand(client_id);
        else if (*tokenCount > REQUEST_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, REMOVE_COMMAND) == 0)
    {
        if (*tokenCount == REMOVE_ARGC)
        {
            const char* username = tokens[1];
            return removeCommand(username, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < REMOVE_ARGC)
            RET_DYN(LESS_ARGUMENTS);
        else if (*tokenCount > REMOVE_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, FRIENDS_COMMAND) == 0)
    {
        if (*tokenCount == FRIENDS_ARGC)
            return friendsCommand(client_id);
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < FRIENDS_ARGC)
            RET_DYN(LESS_ARGUMENTS);
        else if (*tokenCount > FRIENDS_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, FRIENDTYPE_COMMAND) == 0)
    {
        if (*tokenCount == FRIENDTYPE_ARGC)
        {
            const char* username = tokens[1];
            const char* type = tokens[2];
            return friendtypeCommand(username, type, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < FRIENDTYPE_ARGC)
            RET_DYN(LESS_ARGUMENTS);
        else if (*tokenCount > FRIENDTYPE_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, POST_COMMAND) == 0)
    {
        if (*tokenCount >= POST_ARGC)
        {
            const char* privacy = tokens[1];
            const char* message = tokensToMessage(tokens, POST_ARGC, tokenCount);
            return postCommand(privacy, message, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < POST_ARGC)
            RET_DYN(LESS_ARGUMENTS);
    }
    else if (strcmp(command, POSTS_COMMAND) == 0)
    {
        if (*tokenCount == POSTS_ARGC)
            return postsCommand(client_id);
        else if (*tokenCount > POSTS_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, CHAT_COMMAND) == 0)
    {
        if (*tokenCount >= CHAT_ARGC)
        {
            const char* username = tokens[1];
            const char* message = tokensToMessage(tokens, CHAT_ARGC, tokenCount);
            return chatCommand(username, message, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < CHAT_ARGC)
            RET_DYN(LESS_ARGUMENTS);
    }
    else if (strcmp(command, CHATS_COMMAND) == 0)
    {
        if (*tokenCount == CHATS_ARGC)
            return chatsCommand(client_id);
        else if (*tokenCount > CHATS_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, SHOWCHAT_COMMAND) == 0)
    {
        if (*tokenCount == SHOWCHAT_ARGC)
        {
            const char* username = tokens[1];
            return showchatCommand(username, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount > SHOWCHAT_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, CREATEGROUP_COMMAND) == 0)
    {
        if (*tokenCount == CREATEGROUP_ARGC)
        {
            const char* groupname = tokens[1];
            return creategroupCommand(groupname, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount > CREATEGROUP_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, INVITE_COMMAND) == 0)
    {
        if (*tokenCount == INVITE_ARGC)
        {
            const char* username = tokens[1];
            const char* groupname = tokens[2];
            return inviteCommand(username, groupname, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < INVITE_ARGC)
            RET_DYN(LESS_ARGUMENTS);
        else if (*tokenCount > INVITE_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, KICK_COMMAND) == 0)
    {
        if (*tokenCount == KICK_ARGC)
        {
            const char* username = tokens[1];
            const char* groupname = tokens[2];
            return kickCommand(username, groupname, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < KICK_ARGC)
            RET_DYN(LESS_ARGUMENTS);
        else if (*tokenCount > KICK_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, LEAVE_COMMAND) == 0)
    {
        if (*tokenCount == LEAVE_ARGC)
        {
            const char* groupname = tokens[1];
            return leaveCommand(groupname, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount > LEAVE_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, MEMBERS_COMMAND) == 0)
    {
        if (*tokenCount == MEMBERS_ARGC)
        {
            const char* groupname = tokens[1];
            return membersCommand(groupname, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount > MEMBERS_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, GROUPS_COMMAND) == 0)
    {
        if (*tokenCount == GROUPS_ARGC)
            return groupsCommand(client_id);
        else if (*tokenCount > GROUPS_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, GROUPCHAT_COMMAND) == 0)
    {
        if (*tokenCount >= GROUPCHAT_ARGC)
        {
            const char* groupname = tokens[1];
            const char* message = tokensToMessage(tokens, GROUPCHAT_ARGC, tokenCount);
            return groupchatCommand(groupname, message, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount < GROUPCHAT_ARGC)
            RET_DYN(LESS_ARGUMENTS);
    }
    else if (strcmp(command, SHOWGROUPCHAT_COMMAND) == 0)
    {
        if (*tokenCount == SHOWGROUPCHAT_ARGC)
        {
            const char* groupname = tokens[1];
            return showgroupchatCommand(groupname, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount > SHOWGROUPCHAT_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, PRIVACY_COMMAND) == 0)
    {
        if (*tokenCount == PRIVACY_ARGC)
        {
            const char* type = tokens[1];
            return privacyCommand(type, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount > PRIVACY_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, USER_COMMAND) == 0)
    {
        if (*tokenCount == USER_ARGC)
            return userCommand(client_id);
        else if (*tokenCount > USER_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, BAN_COMMAND) == 0)
    {
        if (*tokenCount == BAN_ARGC)
        {
            const char* username = tokens[1];
            return banCommand(username, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount > BAN_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, SEARCH_COMMAND) == 0)
    {
        if (*tokenCount == SEARCH_ARGC)
        {
            const char* username = tokens[1];
            return searchCommand(username, client_id);
        }
        else if (*tokenCount == 0)
            RET_DYN(NO_ARGUMENTS);
        else if (*tokenCount > SEARCH_ARGC)
            RET_DYN(MORE_ARGUMENTS);
    }
    else if (strcmp(command, HELP_COMMAND) == 0)
    {
        if (*tokenCount > HELP_ARGC)
            RET_DYN(MORE_ARGUMENTS);
        else
            RET_DYN(HELP);
    }
    RET_DYN(UNKNOWN_COMMAND);
}

void* handleClient(void* arg)
{
    int clientfd = *(int*) arg;

    char* bufferIn = NULL;

    int tokenCount;
    char** tokens = NULL;

    char* bufferOut = NULL;

    int client_id = LOGGED_OUT;
    free(arg);

    printf("[server] Handling client with fd: %d\n", clientfd);

    while (1)
    {
        int bufferSize = 0;
        if (read(clientfd, &bufferSize, sizeof(int)) <= 0)
        {
            handleThreadError("read(size)");
            break;
        }
        bufferIn = malloc(bufferSize);
        if (read(clientfd, bufferIn, bufferSize) <= 0)
        {
            handleThreadError("read(buffer)");
            break;
        }

        /* ------------------------------------------------------------------ */

        tokenCount = 0;
        tokens = parseCommand(bufferIn, &tokenCount);
        //printf("[d_server] after parseCommand()\n");
        if (bufferIn != NULL)
        {
            free(bufferIn);
            bufferIn = NULL;
        }

        bufferOut = handleCommand(tokens, &tokenCount, &client_id);
        //printf("[d_server] after handleCommand()\n");
        if (tokens != NULL)
        {
            freeTokens(tokens, tokenCount);
            tokens = NULL;
        }

        bufferSize = strlen(bufferOut) + 1;
        printf("[d_server] bufferOut / bufferSize : %s / %d\n", bufferOut, bufferSize);

        /* ------------------------------------------------------------------ */

        if (write(clientfd, &bufferSize, sizeof(int)) < 0)
        {
            handleThreadError("write(size)");
            break;
        }
        if (write(clientfd, bufferOut, bufferSize) < 0)
        {
            handleThreadError("write(buffer)");
            break;
        }
        if (bufferOut != NULL)
        {
            free(bufferOut);
            bufferOut = NULL;
        }
    }

    if (bufferIn != NULL)
    {
        free(bufferIn);
        bufferIn = NULL;
    }
    
    if (tokens != NULL)
    {
        freeTokens(tokens, tokenCount);
        tokens = NULL;
    }

    if (bufferOut != NULL)
    {
        free(bufferOut);
        bufferOut = NULL;
    }

    close(clientfd);
}

int main(void)
{
    struct sockaddr_in server, client;
    int sd;
    const int optval = 1;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handleError("socket()");

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr*) &server, sizeof(server)) < 0)
        handleError("bind()");

    if (listen(sd, 5) < 0)
        handleError("listen()");

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        handleError("signal()");

    printf("[server] Serverul este pornit!\n");

    initDatabase();

    while (1)
    {
        pthread_t thId;

        int cl;
        socklen_t length = sizeof(client);
        if ((cl = accept(sd, (struct sockaddr*) &client, &length)) < 0)
            handleError("accept()");
        
        int* pointercl = malloc(sizeof(int));
        *pointercl = cl;

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&thId, &attr, handleClient, pointercl);
        pthread_attr_destroy(&attr);
    }
}