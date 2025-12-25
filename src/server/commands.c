#pragma once
#include "../../headers/commands.h"
#include "../../headers/structs.h"
#include "../../headers/defines.h"

#include <stdlib.h>
#include <pthread.h>
#include <sqlite3.h>
#include <string.h>
#include <stdio.h>

pthread_mutex_t g_users_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_friends_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_posts_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_chats_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_groups_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_groupmembers_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_groupmessages_mutex = PTHREAD_MUTEX_INITIALIZER;

const char* generateOutput(const char* buffer)
{
    char* bufferOut = malloc(sizeof(OK_RECEIVED) + strlen(buffer));
    sprintf(bufferOut, "%s%s", OK_RECEIVED, buffer);
    return bufferOut;
}

int userExists(int id)
{
    int exists = 0;
    sqlite3_stmt* check;
    const char* sql = "SELECT 1 FROM users WHERE id = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &check, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(check, 1, id);

        pthread_mutex_lock(&g_users_mutex);

        if (sqlite3_step(check) == SQLITE_ROW)
            exists = 1;
        sqlite3_finalize(check);

        pthread_mutex_unlock(&g_users_mutex);
    }

    return exists;
}

int groupExists(int id)
{
    int exists = 0;
    sqlite3_stmt* check;
    const char* sql = "SELECT 1 FROM groups WHERE id = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &check, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(check, 1, id);

        pthread_mutex_lock(&g_groups_mutex);

        if (sqlite3_step(check) == SQLITE_ROW)
            exists = 1;
        sqlite3_finalize(check);

        pthread_mutex_unlock(&g_groups_mutex);
    }

    return exists;
}

int groupOwnerID(int id)
{
    int ownerID = -1;
    sqlite3_stmt* check;
    const char* sql = "SELECT owner_id FROM groups WHERE id = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &check, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(check, 1, id);

        pthread_mutex_lock(&g_groups_mutex);

        if (sqlite3_step(check) == SQLITE_ROW)
            ownerID = sqlite3_column_int(check, 0);
        sqlite3_finalize(check);

        pthread_mutex_unlock(&g_groups_mutex);
    }

    return ownerID;
}

int isUserInGroup(int client_id, int group_id)
{
    int exists = 0;
    sqlite3_stmt* check;
    const char* sql = "SELECT 1 FROM group_members WHERE user_id = ? AND group_id = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &check, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(check, 1, client_id);
        sqlite3_bind_int(check, 2, group_id);

        pthread_mutex_lock(&g_groups_mutex);
        pthread_mutex_lock(&g_groupmembers_mutex);

        if (sqlite3_step(check) == SQLITE_ROW)
            exists = 1;
        sqlite3_finalize(check);

        pthread_mutex_unlock(&g_groups_mutex);
        pthread_mutex_unlock(&g_groupmembers_mutex);
    }

    return exists;
}

int isUserGroupOwner(int client_id, int group_id)
{
    int exists = 0;
    sqlite3_stmt* check;
    const char* sql = "SELECT 1 FROM groups WHERE owner_id = ? AND id = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &check, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(check, 1, client_id);
        sqlite3_bind_int(check, 2, group_id);

        pthread_mutex_lock(&g_groups_mutex);

        if (sqlite3_step(check) == SQLITE_ROW)
            exists = 1;
        sqlite3_finalize(check);

        pthread_mutex_unlock(&g_groups_mutex);
    }

    return exists;
}

int usernameToId(const char* username)
{
    // userul a dat un username nu un id si facem rost de id dupa username
    int friendID = -1;
    sqlite3_stmt* useridStmt;
    const char* sql_select = "SELECT id FROM users WHERE username = ?";
    int rc = sqlite3_prepare_v2(db, sql_select, -1, &useridStmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return -2;
    }
    sqlite3_bind_text(useridStmt, 1, username, -1, SQLITE_STATIC);
    pthread_mutex_lock(&g_users_mutex);
    rc = sqlite3_step(useridStmt);
    if (rc == SQLITE_ROW)
    {
        friendID = sqlite3_column_int(useridStmt, 0);
        sqlite3_finalize(useridStmt);
        pthread_mutex_unlock(&g_users_mutex);
        return friendID;
    }
    else if (rc == SQLITE_DONE)
    {
        sqlite3_finalize(useridStmt);
        pthread_mutex_unlock(&g_users_mutex);
        return friendID;
    }
    else
    {
        printf("[server] Eroare la sqlite3_step()\n");
        sqlite3_finalize(useridStmt);
        pthread_mutex_unlock(&g_users_mutex);
        return -2;
    }
}

int groupnameToId(const char* groupname, int* client_id)
{
    // userul a dat un groupname nu un id si facem rost de id dupa groupname
    int groupID = -1;
    sqlite3_stmt* groupidStmt;
    const char* sql_select = "SELECT id FROM groups WHERE name = ?";
    int rc = sqlite3_prepare_v2(db, sql_select, -1, &groupidStmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return -2;
    }
    sqlite3_bind_text(groupidStmt, 1, groupname, -1, SQLITE_STATIC);
    pthread_mutex_lock(&g_groups_mutex);
    rc = sqlite3_step(groupidStmt);
    if (rc == SQLITE_ROW)
    {
        groupID = sqlite3_column_int(groupidStmt, 0);
        sqlite3_finalize(groupidStmt);
        pthread_mutex_unlock(&g_groups_mutex);
        return groupID;
    }
    else if (rc == SQLITE_DONE)
    {
        sqlite3_finalize(groupidStmt);
        pthread_mutex_unlock(&g_groups_mutex);
        return groupID;
    }
    else
    {
        printf("[server] Eroare la sqlite3_step()\n");
        sqlite3_finalize(groupidStmt);
        pthread_mutex_unlock(&g_groups_mutex);
        return -2;
    }
}

unsigned char* idToUsername(int id)
{
    unsigned char* username;
    sqlite3_stmt* useridStmt;
    const char* sql_select = "SELECT username FROM users WHERE id = ?";
    int rc = sqlite3_prepare_v2(db, sql_select, -1, &useridStmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(useridStmt, 1, id);
    pthread_mutex_lock(&g_users_mutex);
    rc = sqlite3_step(useridStmt);
    if (rc == SQLITE_ROW)
    {
        const unsigned char* tempUsername = sqlite3_column_text(useridStmt, 0);
        username = malloc(strlen(tempUsername) + 1);
        strcpy(username, tempUsername);
        sqlite3_finalize(useridStmt);
        pthread_mutex_unlock(&g_users_mutex);
        return username;
    }
    else
    {
        printf("[server] Eroare la sqlite3_step()\n");
        sqlite3_finalize(useridStmt);
        pthread_mutex_unlock(&g_users_mutex);
        return SQL_ERROR;
    }
}

unsigned char* idToGroupname(int id)
{
    unsigned char* groupname;
    sqlite3_stmt* groupidStmt;
    const char* sql_select = "SELECT name FROM groups WHERE id = ?";
    int rc = sqlite3_prepare_v2(db, sql_select, -1, &groupidStmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(groupidStmt, 1, id);
    pthread_mutex_lock(&g_groups_mutex);
    rc = sqlite3_step(groupidStmt);
    if (rc == SQLITE_ROW)
    {
        const unsigned char* tempGroupname = sqlite3_column_text(groupidStmt, 0);
        groupname = malloc(strlen(tempGroupname) + 1);
        strcpy(groupname, tempGroupname);
        sqlite3_finalize(groupidStmt);
        pthread_mutex_unlock(&g_groups_mutex);
        return groupname;
    }
    else
    {
        printf("[server] Eroare la sqlite3_step()\n");
        sqlite3_finalize(groupidStmt);
        pthread_mutex_unlock(&g_groups_mutex);
        return SQL_ERROR;
    }
}

/*                          start of commands                           */

const char* registerCommand(const char* username, const char* password, const char* type)
{
    if (strlen(username) >= usernameLength)
    {
        return USERNAME_TOO_LONG;
    }
    if (strlen(password) >= passwordLength)
    {
        return PASSWORD_TOO_LONG;
    }
    if (strcmp(type, REGULAR_USER) != 0 && strcmp(type, ADMIN_USER) != 0)
    {
        return INVALID_TYPE;
    }

    int int_type = 0;
    if (strcmp(type, REGULAR_USER) == 0)
        int_type = regular_user;
    else
        int_type = admin_user;

    sqlite3_stmt* stmt;
    const char* sql_insert = "INSERT INTO users (username, password, role) VALUES (?, ?, ?)";
    int rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la register\n");
        return SQL_ERROR;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, int_type);

    pthread_mutex_lock(&g_users_mutex);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_users_mutex);

    if (rc == SQLITE_DONE)
        return REGISTER_SUCCESS;
    else if (rc == SQLITE_CONSTRAINT)
        return REGISTER_FAILED;
    else
    {
        printf("[server] Eroare la sqlite3_step() la register\n");
        return SQL_ERROR;
    }
}

const char* loginCommand(const char* username, const char* password, int* client_id)
{
    if (*client_id != LOGGED_OUT)
    {
        return LOGIN_ALREADY;
    }
    if (strlen(username) >= usernameLength)
    {
        return USERNAME_TOO_LONG;
    }
    if (strlen(password) >= passwordLength)
    {
        return PASSWORD_TOO_LONG;
    }

    sqlite3_stmt* stmt;
    const char* sql_select = "SELECT id FROM users WHERE username = ? AND password = ?";
    int rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la login\n");
        return SQL_ERROR;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    
    pthread_mutex_lock(&g_users_mutex);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        *client_id = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        pthread_mutex_unlock(&g_users_mutex);
        return LOGIN_SUCCESS;
    }
    else if (rc == SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        pthread_mutex_unlock(&g_users_mutex);
        return LOGIN_FAILED;
    }
    else
    {
        printf("[server] Eroare la sqlite3_step() la login\n");
        sqlite3_finalize(stmt);
        pthread_mutex_unlock(&g_users_mutex);
        return SQL_ERROR;
    }
    
}

const char* logoutCommand(int* client_id)
{
    if (*client_id != LOGGED_OUT)
    {
        *client_id = LOGGED_OUT;
        return LOGOUT_SUCCESS;
    }
    else
    {
        return NOT_LOGGED;
    }
}

const char* addCommand(const char* username, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int friendID = strtol(username, &endptr, 10);
    int rc;

    if (*endptr != '\0') // userul a dat un username nu un id si facem rost de id dupa username
    {
        friendID = usernameToId(username);
        if (friendID == -1)
            return ADD_FAILED;
        else if (friendID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!userExists(friendID))
            return ADD_FAILED;
    }

    if (*client_id == friendID)
        return ADD_SELF;
    
    sqlite3_stmt* check_added;
    const char* sql_check = "SELECT status, user_id FROM friends WHERE ((user_id = ? AND friend_id = ?) OR (user_id = ? AND friend_id = ?))";
    rc = sqlite3_prepare_v2(db, sql_check, -1, &check_added, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(check_added, 1, *client_id);
    sqlite3_bind_int(check_added, 2, friendID);
    sqlite3_bind_int(check_added, 3, friendID);
    sqlite3_bind_int(check_added, 4, *client_id);

    int status, sender;
    pthread_mutex_lock(&g_friends_mutex);

    if ((rc = sqlite3_step(check_added)) == SQLITE_ROW)
    {
        status = sqlite3_column_int(check_added, 0);
        sender = sqlite3_column_int(check_added, 1);
    }
    sqlite3_finalize(check_added);

    pthread_mutex_unlock(&g_friends_mutex);

    if (rc == SQLITE_ROW)
    {
        if (status == 1)
            return ADD_ALREADY_FRIENDS;
        else
        {
            if (sender == *client_id)
                return ADD_YOU;
            else
                return ADD_OTHER;
        }
    }
    
    sqlite3_stmt* stmt;
    const char* sql_insert = "INSERT INTO friends (user_id, friend_id) VALUES (?, ?)";
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, *client_id);
    sqlite3_bind_int(stmt, 2, friendID);

    pthread_mutex_lock(&g_friends_mutex);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_friends_mutex);

    if (rc == SQLITE_DONE)
        return ADD_SUCCESS;
    else if (rc == SQLITE_CONSTRAINT)
        return ADD_FAILED;
    else
    {
        printf("[server] Eroare la sqlite3_step() la add\n");
        return SQL_ERROR;
    }
}

const char* acceptCommand(const char* username, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int friendID = strtol(username, &endptr, 10);
    int rc;

    if (*endptr != '\0') // userul a dat un username nu un id si facem rost de id dupa username
    {
        friendID = usernameToId(username);
        if (friendID == -1)
            return ADD_FAILED;
        else if (friendID == -2)
            return SQL_ERROR;
    }

    if (*client_id == friendID)
        return REQUEST_SELF;
    
    sqlite3_stmt* stmt;
    const char* sql_update = "UPDATE friends SET status = 1 WHERE user_id = ? AND friend_id = ? AND status = 0";
    rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la add\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, friendID);
    sqlite3_bind_int(stmt, 2, *client_id);

    int modifyCount;

    pthread_mutex_lock(&g_friends_mutex);

    rc = sqlite3_step(stmt);
    modifyCount = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_friends_mutex);

    if (modifyCount > 0 && rc == SQLITE_DONE)
        return REQUEST_ACCEPT_SUCCESS;
    else if (modifyCount == 0 && rc == SQLITE_DONE)
        return REQUEST_FAILED;
    else
    {
        printf("[server] Eroare la sqlite3_step() la add\n");
        return SQL_ERROR;
    }
}

const char* declineCommand(const char* username, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int friendID = strtol(username, &endptr, 10);
    int rc;

    if (*endptr != '\0') // userul a dat un username nu un id si facem rost de id dupa username
    {
        friendID = usernameToId(username);
        if (friendID == -1)
            return ADD_FAILED;
        else if (friendID == -2)
            return SQL_ERROR;
    }

    if (*client_id == friendID)
        return REQUEST_SELF;

    sqlite3_stmt* stmt;
    const char* sql_delete = "DELETE FROM friends WHERE ((user_id = ? AND friend_id = ?) OR (user_id = ? AND friend_id = ?)) AND status = 0";
    rc = sqlite3_prepare_v2(db, sql_delete, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la add\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, friendID);
    sqlite3_bind_int(stmt, 2, *client_id);
    sqlite3_bind_int(stmt, 3, *client_id);
    sqlite3_bind_int(stmt, 4, friendID);

    int modifyCount;

    pthread_mutex_lock(&g_friends_mutex);

    rc = sqlite3_step(stmt);
    modifyCount = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_friends_mutex);

    if (modifyCount > 0 && rc == SQLITE_DONE)
        return REQUEST_DECLINE_SUCCESS;
    else if (rc == SQLITE_DONE)
        return REQUEST_FAILED;
    else
    {
        printf("[server] Eroare la sqlite3_step() la add\n");
        return SQL_ERROR;
    }
}

const char* requestsCommand(int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    int rc;
    
    sqlite3_stmt* stmt;
    const char* sql_select = "SELECT u.id, u.username FROM users u JOIN friends f ON u.id = f.user_id WHERE f.status = 0 AND f.friend_id = ?";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la REQUESTS\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(stmt, 1, *client_id);

    int inrequestsSize = sizeof(REQUESTS_IN);
    char* inrequests = malloc(inrequestsSize);
    strcpy(inrequests, REQUESTS_IN);

    //Incoming Requests
    pthread_mutex_lock(&g_friends_mutex);
    pthread_mutex_lock(&g_users_mutex);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* username = sqlite3_column_text(stmt, 1);

        char request[BUFFER_SIZE];
        snprintf(request, sizeof(request), "\n[%d] %s", id, username);

        inrequestsSize += strlen(request);
        inrequests = realloc(inrequests, inrequestsSize);
        strcat(inrequests, request);
    }
    sqlite3_finalize(stmt);
    
    pthread_mutex_unlock(&g_friends_mutex);
    pthread_mutex_unlock(&g_users_mutex);

    if (inrequestsSize == sizeof(REQUESTS_IN))
    {
        inrequestsSize = sizeof(REQUESTS_NONE_IN);
        inrequests = realloc(inrequests, inrequestsSize);
        strcpy(inrequests, REQUESTS_NONE_IN);
    }

    //Outgoing requests
    const char* sql_outgoing = "SELECT u.id, u.username FROM users u JOIN friends f ON u.id = f.friend_id WHERE f.status = 0 AND f.user_id = ?";
    rc = sqlite3_prepare_v2(db, sql_outgoing, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        free(inrequests);
        printf("[server] Eroare sql_prepare() la REQUESTS2\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(stmt, 1, *client_id);

    int outrequestsSize = sizeof(REQUESTS_OUT);
    char* outrequests = malloc(outrequestsSize);
    strcpy(outrequests, REQUESTS_OUT);

    pthread_mutex_lock(&g_friends_mutex);
    pthread_mutex_lock(&g_users_mutex);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* username = sqlite3_column_text(stmt, 1);

        char request[BUFFER_SIZE];
        snprintf(request, sizeof(request), "\n[%d] %s", id, username);

        outrequestsSize += strlen(request);
        outrequests = realloc(outrequests, outrequestsSize);
        strcat(outrequests, request);
    }
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_friends_mutex);
    pthread_mutex_unlock(&g_users_mutex);

    if (outrequestsSize == sizeof(REQUESTS_OUT))
    {
        outrequestsSize = sizeof(REQUESTS_NONE_OUT);
        outrequests = realloc(outrequests, outrequestsSize);
        strcpy(outrequests, REQUESTS_NONE_OUT);
    }

    char* requests = malloc(inrequestsSize + outrequestsSize - 1);
    strcpy(requests, inrequests);
    strcat(requests, outrequests);
    free(inrequests);
    free(outrequests);

    if (rc == SQLITE_DONE && (inrequestsSize > sizeof(REQUESTS_NONE_IN) || outrequestsSize > sizeof(REQUESTS_NONE_OUT)))
        return requests;
    else if (rc == SQLITE_DONE && inrequestsSize == sizeof(REQUESTS_NONE_IN) && outrequestsSize == sizeof(REQUESTS_NONE_OUT))
    {
        free(requests);
        return REQUESTS_NONE;
    }
    else
    {
        free(requests);
        printf("[server] Eroare la sqlite3_step() la REQUESTS3\n");
        return SQL_ERROR;
    }
}

const char* removeCommand(const char* username, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int friendID = strtol(username, &endptr, 10);
    int rc;

    if (*endptr != '\0') // userul a dat un username nu un id si facem rost de id dupa username
    {
        friendID = usernameToId(username);
        if (friendID == -1)
            return ADD_FAILED;
        else if (friendID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!userExists(friendID))
            return ADD_FAILED;
    }

    if (*client_id == friendID)
        return REMOVE_SELF;
    
    sqlite3_stmt* remove_friend;
    const char* sql_remove = "DELETE FROM friends WHERE ((user_id = ? AND friend_id = ?) OR (user_id = ? AND friend_id = ?)) AND status = 1";
    rc = sqlite3_prepare_v2(db, sql_remove, -1, &remove_friend, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(remove_friend, 1, *client_id);
    sqlite3_bind_int(remove_friend, 2, friendID);
    sqlite3_bind_int(remove_friend, 3, friendID);
    sqlite3_bind_int(remove_friend, 4, *client_id);

    int modifyCount = 0;

    pthread_mutex_lock(&g_friends_mutex);

    rc = sqlite3_step(remove_friend);
    modifyCount = sqlite3_changes(db);
    sqlite3_finalize(remove_friend);

    pthread_mutex_unlock(&g_friends_mutex);

    if (modifyCount > 0 && rc == SQLITE_DONE)
        return REMOVE_SUCCESS;
    else if (rc == SQLITE_DONE)
        return REMOVE_FAILED;
    else
    {
        printf("[server] Eroare la sqlite3_step() la remove\n");
        return SQL_ERROR;
    }
}

const char* friendsCommand(int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    int rc;
    
    sqlite3_stmt* stmt;
    const char* sql_select = "SELECT u.id, u.username, f.type FROM users u JOIN friends f ON (u.id = f.user_id AND f.friend_id = ?) OR (u.id = f.friend_id AND f.user_id = ?) WHERE f.status = 1";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la friends\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(stmt, 1, *client_id);
    sqlite3_bind_int(stmt, 2, *client_id);

    int friendsSize = sizeof(FRIENDS);
    char* friends = malloc(friendsSize);
    strcpy(friends, FRIENDS);

    pthread_mutex_lock(&g_friends_mutex);
    pthread_mutex_lock(&g_users_mutex);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* username = sqlite3_column_text(stmt, 1);
        int type = sqlite3_column_int(stmt, 2);
        const char* type_text = type == 0 ? "REGULAR" : "CLOSE";

        char friend[BUFFER_SIZE];
        snprintf(friend, sizeof(friend), "\n[%d] %s | %s", id, username, type_text);

        friendsSize += strlen(friend);
        friends = realloc(friends, friendsSize);
        strcat(friends, friend);
    }
    sqlite3_finalize(stmt);
    
    pthread_mutex_unlock(&g_friends_mutex);
    pthread_mutex_unlock(&g_users_mutex);

    if (friendsSize == sizeof(FRIENDS))
    {
        friends = realloc(friends, sizeof(FRIENDS_FAILED));
        strcpy(friends, FRIENDS_FAILED);
    }

    if (rc == SQLITE_DONE)
        return friends;
    else
    {
        free(friends);
        printf("[server] Eroare la sqlite3_step() la friends\n");
        return SQL_ERROR;
    }
}

const char* friendtypeCommand(const char* username, const char* type, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;

    char* endptr;
    int friendID = strtol(username, &endptr, 10);
    int rc;

    if (*endptr != '\0') // userul a dat un username nu un id si facem rost de id dupa username
    {
        friendID = usernameToId(username);
        if (friendID == -1)
            return ADD_FAILED;
        else if (friendID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!userExists(friendID))
            return ADD_FAILED;
    }

    if (*client_id == friendID)
        return FRIENDTYPE_SELF;

    if (strcmp(type, REGULAR_FRIEND) != 0 && strcmp(type, CLOSE_FRIEND) != 0)
    {
        return INVALID_FRIEND_TYPE;
    }

    int int_type = 0;
    if (strcmp(type, REGULAR_FRIEND) == 0)
        int_type = regular_friend;
    else
        int_type = close_friend;

    sqlite3_stmt* stmt;
    const char* sql_update = "UPDATE friends SET type = ? WHERE ((user_id = ? AND friend_id = ?) OR (user_id = ? AND friend_id = ?)) AND status = 1";
    rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la friendtype\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, int_type);
    sqlite3_bind_int(stmt, 2, *client_id);
    sqlite3_bind_int(stmt, 3, friendID);
    sqlite3_bind_int(stmt, 4, friendID);
    sqlite3_bind_int(stmt, 5, *client_id);

    int modifyCount = 0;

    pthread_mutex_lock(&g_friends_mutex);

    rc = sqlite3_step(stmt);
    modifyCount = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_friends_mutex);

    if (modifyCount > 0 && rc == SQLITE_DONE)
        return FRIENDTYPE_SUCCESS;
    else if (rc == SQLITE_DONE)
        return FRIENDTYPE_FAILED;
    else
    {
        printf("[server] Eroare la sqlite3_step() la friendtype\n");
        return SQL_ERROR;
    }
}

const char* postCommand(const char* privacy, const char* message, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    int rc;
    int int_privacy;
    if (strcmp(privacy, POST_PUBLIC) == 0)
        int_privacy = public_post;
    else if (strcmp(privacy, POST_REGULAR) == 0)
        int_privacy = regular_post;
    else if (strcmp(privacy, POST_CLOSE) == 0)
        int_privacy = close_post;
    else
        return INVALID_POST_TYPE;

    sqlite3_stmt* stmt;
    const char* sql_insert = "INSERT into POSTS (user_id, content, visibility) VALUES (?, ?, ?)";
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, *client_id);
    sqlite3_bind_text(stmt, 2, message, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, int_privacy);

    pthread_mutex_lock(&g_posts_mutex);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_posts_mutex);

    if (rc == SQLITE_DONE)
        return POST_SUCCESS;
    else
    {
        printf("[server] Eroare sqlite3_step() la post\n");
        return SQL_ERROR;
    }
}

const char* postsCommand(int* client_id)
{   
    int rc;
    sqlite3_stmt* stmt;
    const char* sql_select;
    if (*client_id == LOGGED_OUT)
    {
        //daca nu e logat, doar cele publice de la userii publici
        sql_select = "SELECT u.id, u.username, p.content, p.visibility, p.created_at FROM users u "
                        "JOIN posts p ON u.id = p.user_id AND p.visibility = 0 AND u.privacy = 0";
        rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    }
    else
    {
        //daca e logat, cele unde friends.type + 1 >= posts.visibility (daca e close friend le vede si pe cele regular, invers nu) 
        sql_select = "SELECT u.id, u.username, p.content, p.visibility, p.created_at FROM users u "
                        "JOIN posts p ON u.id = p.user_id "
                        "LEFT JOIN friends f ON ((f.user_id = ? AND f.friend_id = u.id) OR (f.user_id = u.id AND f.friend_id = ?)) AND f.status = 1 "
                        "WHERE p.user_id = ? "
                        "OR (p.visibility = 0 AND (f.user_id IS NOT NULL OR u.privacy = 0)) "
                        "OR (f.user_id IS NOT NULL AND f.type + 1 >= p.visibility)";
        rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
        sqlite3_bind_int(stmt, 1, *client_id);
        sqlite3_bind_int(stmt, 2, *client_id);
        sqlite3_bind_int(stmt, 3, *client_id);
    }

    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la friends\n");
        return SQL_ERROR;
    }

    int postsSize = sizeof(POSTS);
    char* posts = malloc(postsSize);
    strcpy(posts, POSTS);

    pthread_mutex_lock(&g_friends_mutex);
    pthread_mutex_lock(&g_posts_mutex);
    pthread_mutex_lock(&g_users_mutex);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int user_id = sqlite3_column_int(stmt, 0);
        const unsigned char* username = sqlite3_column_text(stmt, 1);
        const unsigned char* content = sqlite3_column_text(stmt, 2);
        int visibility = sqlite3_column_int(stmt, 3);
        const unsigned char* created_at = sqlite3_column_text(stmt, 4);

        const char* visibility_text;
        if (visibility == public_post)
            visibility_text = POST_PUBLIC;
        else if (visibility == regular_post)
            visibility_text = POST_REGULAR;
        else
            visibility_text = POST_CLOSE;

        char post[BUFFER_SIZE];
        snprintf(post, sizeof(post), "\n(%s) [%d] %s | \"%s\" | %s", created_at, user_id, username, content, visibility_text);

        postsSize += strlen(post);
        posts = realloc(posts, postsSize);
        strcat(posts, post);
    }
    sqlite3_finalize(stmt);
    
    pthread_mutex_unlock(&g_friends_mutex);
    pthread_mutex_unlock(&g_posts_mutex);
    pthread_mutex_unlock(&g_users_mutex);

    if (postsSize == sizeof(POSTS))
    {
        posts = realloc(posts, sizeof(POSTS_FAILED));
        strcpy(posts, POSTS_FAILED);
    }

    if (rc == SQLITE_DONE)
        return posts;
    else
    {
        free(posts);
        printf("[server] Eroare la sqlite3_step() la friends\n");
        return SQL_ERROR;
    }
}

const char* chatCommand(const char* username, const char* message, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int receiverID = strtol(username, &endptr, 10);
    int rc;

    if (*endptr != '\0') // userul a dat un username nu un id si facem rost de id dupa username
    {
        receiverID = usernameToId(username);
        if (receiverID == -1)
            return ADD_FAILED;
        else if (receiverID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!userExists(receiverID))
            return ADD_FAILED;
    }

    if (*client_id == receiverID)
        return CHAT_SELF;
    
    sqlite3_stmt* stmt;
    const char* sql_insert = "INSERT into messages (sender_id, receiver_id, message) VALUES (?, ?, ?)";
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, *client_id);
    sqlite3_bind_int(stmt, 2, receiverID);
    sqlite3_bind_text(stmt, 3, message, -1, SQLITE_STATIC);

    pthread_mutex_lock(&g_chats_mutex);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_chats_mutex);

    if (rc == SQLITE_DONE)
        return CHAT_SUCCESS;
    else
    {
        printf("[server] Eroare sqlite3_step() la post\n");
        return SQL_ERROR;
    }
}

const char* chatsCommand(int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    int rc;
    
    sqlite3_stmt* stmt;
    const char* sql_select = "SELECT u.id, u.username, m.message, m.created_at, m.sender_id FROM users u "
                                "JOIN messages m ON u.id = (CASE WHEN m.sender_id = ? THEN m.receiver_id ELSE m.sender_id END) "
                                "WHERE m.id IN ("
                                "SELECT MAX(id) FROM messages "
                                "WHERE sender_id = ? or receiver_id = ? "
                                "GROUP BY CASE WHEN sender_id = ? THEN receiver_id ELSE sender_id END) "
                                "ORDER BY m.created_at DESC";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la friends\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(stmt, 1, *client_id);
    sqlite3_bind_int(stmt, 2, *client_id);
    sqlite3_bind_int(stmt, 3, *client_id);
    sqlite3_bind_int(stmt, 4, *client_id);

    int chatsSize = sizeof(CHATS);
    char* chats = malloc(chatsSize);
    strcpy(chats, CHATS);

    pthread_mutex_lock(&g_chats_mutex);
    pthread_mutex_lock(&g_users_mutex);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* username = sqlite3_column_text(stmt, 1);
        const unsigned char* message = sqlite3_column_text(stmt, 2);
        const unsigned char* created_at = sqlite3_column_text(stmt, 3);
        int sender_id = sqlite3_column_int(stmt, 4);

        const char* prefix = "";
        if (sender_id == *client_id)
            prefix = "You: ";

        char chat[BUFFER_SIZE];
        snprintf(chat, sizeof(chat), "\n(%s) [%d] %s | %s%.16s...", created_at, id, username, prefix, message);

        chatsSize += strlen(chat);
        chats = realloc(chats, chatsSize);
        strcat(chats, chat);
    }
    sqlite3_finalize(stmt);
    
    pthread_mutex_unlock(&g_chats_mutex);
    pthread_mutex_unlock(&g_users_mutex);

    if (chatsSize == sizeof(CHATS))
    {
        chats = realloc(chats, sizeof(CHATS_FAILED));
        strcpy(chats, CHATS_FAILED);
    }
    else
    {
        chats = realloc(chats, chatsSize + sizeof(CHATS_SUFFIX) - 1);
        strcat(chats, CHATS_SUFFIX);
    }

    if (rc == SQLITE_DONE)
        return chats;
    else
    {
        free(chats);
        printf("[server] Eroare la sqlite3_step() la friends\n");
        return SQL_ERROR;
    }
}

const char* showchatCommand(const char* username, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;

    char* endptr;
    int friendID = strtol(username, &endptr, 10);
    int rc;

    if (*endptr != '\0') // userul a dat un username nu un id si facem rost de id dupa username
    {
        friendID = usernameToId(username);
        if (friendID == -1)
            return ADD_FAILED;
        else if (friendID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!userExists(friendID))
            return ADD_FAILED;
    }

    if (*client_id == friendID)
        return SHOWCHAT_SELF;
    
    sqlite3_stmt* stmt;
    const char* sql_select = "SELECT message, created_at, sender_id FROM messages "
                                "WHERE (sender_id = ? AND receiver_id = ?) OR (sender_id = ? AND receiver_id = ?) "
                                "ORDER BY created_at ASC";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la friends\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(stmt, 1, *client_id);
    sqlite3_bind_int(stmt, 2, friendID);
    sqlite3_bind_int(stmt, 3, friendID);
    sqlite3_bind_int(stmt, 4, *client_id);

    unsigned char* friendUsername = idToUsername(friendID);
    int initSize;
    int chatsSize = initSize = sizeof(SHOWCHAT) + strlen(friendUsername) + 1;
    char* chats = malloc(chatsSize);
    strcpy(chats, SHOWCHAT);
    strcat(strcat(chats, friendUsername), ":");

    pthread_mutex_lock(&g_chats_mutex);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int id;
        const unsigned char* username;
        const unsigned char* message = sqlite3_column_text(stmt, 0);
        const unsigned char* created_at = sqlite3_column_text(stmt, 1);
        int sender_id = sqlite3_column_int(stmt, 2);

        if (sender_id == *client_id)
        {
            id = *client_id;
            username = "You: ";
        }
        else
        {
            id = friendID;
            username = friendUsername;
        }

        char chat[BUFFER_SIZE];
        snprintf(chat, sizeof(chat), "\n(%s) [%d] %s | %s", created_at, id, username, message);

        chatsSize += strlen(chat);
        chats = realloc(chats, chatsSize);
        strcat(chats, chat);
    }
    sqlite3_finalize(stmt);
    
    pthread_mutex_unlock(&g_chats_mutex);

    free(friendUsername);

    if (chatsSize == initSize)
    {
        chats = realloc(chats, sizeof(CHATS_FAILED));
        strcpy(chats, CHATS_FAILED);
    }

    if (rc == SQLITE_DONE)
        return chats;
    else
    {
        free(chats);
        printf("[server] Eroare la sqlite3_step() la friends\n");
        return SQL_ERROR;
    }
}

const char* creategroupCommand(const char* groupname, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    if (strlen(groupname) >= groupnameLength)
    {
        return GROUPNAME_TOO_LONG;
    }

    sqlite3_stmt* stmt;
    const char* sql_insert = "INSERT INTO groups (name, owner_id) VALUES (?, ?);";
    int rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la register\n");
        return SQL_ERROR;
    }

    sqlite3_bind_text(stmt, 1, groupname, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, *client_id);

    pthread_mutex_lock(&g_groups_mutex);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    int groupID = sqlite3_last_insert_rowid(db);

    pthread_mutex_unlock(&g_groups_mutex);

    sql_insert = "INSERT INTO group_members (user_id, group_id) VALUES (?, ?)";
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, *client_id);
    sqlite3_bind_int(stmt, 2, groupID);

    pthread_mutex_lock(&g_groupmembers_mutex);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_groupmembers_mutex);
    

    if (rc == SQLITE_DONE)
        return CREATEGROUP_SUCCESS;
    else if (rc == SQLITE_CONSTRAINT)
        return CREATEGROUP_FAILED;
    else
    {
        printf("[server] Eroare la sqlite3_step() la register\n");
        return SQL_ERROR;
    }
}

const char* inviteCommand(const char* username, const char* groupname, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int friendID = strtol(username, &endptr, 10);
    int rc;

    if (*endptr != '\0') // userul a dat un username nu un id si facem rost de id dupa username
    {
        friendID = usernameToId(username);
        if (friendID == -1)
            return ADD_FAILED;
        else if (friendID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!userExists(friendID))
            return ADD_FAILED;
    }

    if (*client_id == friendID)
        return INVITE_SELF;

    int groupID = strtol(groupname, &endptr, 10);

    if (*endptr != '\0') // userul a dat un groupname nu un id si facem rost de id dupa groupname
    {
        groupID = groupnameToId(groupname, client_id);
        if (groupID == -1)
            return INVITE_GROUPNOTEXIST;
        else if (groupID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!groupExists(groupID))
            return INVITE_GROUPNOTEXIST;
    }

    if (!isUserInGroup(*client_id, groupID))
        return INVITE_FAILED;
    
    sqlite3_stmt* stmt;
    const char* sql_insert = "INSERT INTO group_members (user_id, group_id) VALUES (?, ?)";
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, friendID);
    sqlite3_bind_int(stmt, 2, groupID);

    pthread_mutex_lock(&g_groupmembers_mutex);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_groupmembers_mutex);

    if (rc == SQLITE_DONE)
        return INVITE_SUCCESS;
    else if (rc == SQLITE_CONSTRAINT)
        return INVITE_ALREADY;
    else
    {
        printf("[server] Eroare la sqlite3_step() la add\n");
        return SQL_ERROR;
    }
}

const char* kickCommand(const char* username, const char* groupname, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int friendID = strtol(username, &endptr, 10);
    int rc;

    if (*endptr != '\0') // userul a dat un username nu un id si facem rost de id dupa username
    {
        friendID = usernameToId(username);
        if (friendID == -1)
            return ADD_FAILED;
        else if (friendID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!userExists(friendID))
            return ADD_FAILED;
    }

    if (*client_id == friendID)
        return KICK_SELF;

    int groupID = strtol(groupname, &endptr, 10);

    if (*endptr != '\0') // userul a dat un groupname nu un id si facem rost de id dupa groupname
    {
        groupID = groupnameToId(groupname, client_id);
        if (groupID == -1)
            return KICK_GROUPNOTEXIST;
        else if (groupID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!groupExists(groupID))
            return KICK_GROUPNOTEXIST;
    }

    if (!isUserInGroup(*client_id, groupID))
        return KICK_FAILED;

    if (!isUserGroupOwner(*client_id, groupID))
        return KICK_NOTOWNER;
    
    sqlite3_stmt* stmt;
    const char* sql_delete = "DELETE FROM group_members WHERE user_id = ? AND group_id = ?";
    rc = sqlite3_prepare_v2(db, sql_delete, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, friendID);
    sqlite3_bind_int(stmt, 2, groupID);

    int modifyCount;

    pthread_mutex_lock(&g_groupmembers_mutex);

    rc = sqlite3_step(stmt);
    modifyCount = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_groupmembers_mutex);

    if (modifyCount > 0 && rc == SQLITE_DONE)
        return KICK_SUCCESS;
    else if (rc == SQLITE_DONE)
        return KICK_ALREADY;
    else
    {
        printf("[server] Eroare la sqlite3_step() la add\n");
        return SQL_ERROR;
    }
}

const char* leaveCommand(const char* groupname, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int rc;
    int groupID = strtol(groupname, &endptr, 10);
    int isOwner = 0;

    if (*endptr != '\0') // userul a dat un groupname nu un id si facem rost de id dupa groupname
    {
        groupID = groupnameToId(groupname, client_id);
        if (groupID == -1)
            return KICK_GROUPNOTEXIST;
        else if (groupID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!groupExists(groupID))
            return KICK_GROUPNOTEXIST;
    }

    if (!isUserInGroup(*client_id, groupID))
        return KICK_FAILED;

    isOwner = isUserGroupOwner(*client_id, groupID);
    
    sqlite3_stmt* stmt;
    const char* sql_delete = "DELETE FROM group_members WHERE user_id = ? AND group_id = ?";
    rc = sqlite3_prepare_v2(db, sql_delete, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, *client_id);
    sqlite3_bind_int(stmt, 2, groupID);

    pthread_mutex_lock(&g_groupmembers_mutex);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_groupmembers_mutex);

    //daca iese ownerul dam ownership la un membru, daca nu mai sunt membrii stergem grupul
    if (isOwner)
    {
        int newOwnerID = -1;
        const char* sql = "SELECT user_id FROM group_members WHERE group_id = ? LIMIT 1";
        sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
        sqlite3_bind_int(stmt, 1, groupID);

        pthread_mutex_lock(&g_groupmembers_mutex);

        if (sqlite3_step(stmt) == SQLITE_ROW)
            newOwnerID = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);

        pthread_mutex_unlock(&g_groupmembers_mutex);

        if (newOwnerID == -1)
        {
            sql = "DELETE FROM groups WHERE id = ?";
            sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
            sqlite3_bind_int(stmt, 1, groupID);

            pthread_mutex_lock(&g_groups_mutex);

            sqlite3_step(stmt);
            sqlite3_finalize(stmt);

            pthread_mutex_unlock(&g_groups_mutex);
        }
        else
        {
            sql = "UPDATE groups SET owner_id = ? WHERE id = ?";
            sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
            sqlite3_bind_int(stmt, 1, newOwnerID);
            sqlite3_bind_int(stmt, 2, groupID);

            pthread_mutex_lock(&g_groups_mutex);

            sqlite3_step(stmt);
            sqlite3_finalize(stmt);

            pthread_mutex_unlock(&g_groups_mutex);
        }
    }

    if (rc == SQLITE_DONE)
        return LEAVE_SUCCESS;
    else if (rc == SQLITE_CONSTRAINT)
        return LEAVE_FAILED;
    else
    {
        printf("[server] Eroare la sqlite3_step() la add\n");
        return SQL_ERROR;
    }
}

const char* membersCommand(const char* groupname, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int rc;
    int groupID = strtol(groupname, &endptr, 10);

    if (*endptr != '\0') // userul a dat un groupname nu un id si facem rost de id dupa groupname
    {
        groupID = groupnameToId(groupname, client_id);
        if (groupID == -1)
            return KICK_GROUPNOTEXIST;
        else if (groupID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!groupExists(groupID))
            return KICK_GROUPNOTEXIST;
    }

    if (!isUserInGroup(*client_id, groupID))
        return KICK_FAILED;
    
    sqlite3_stmt* stmt;
    const char* sql_select = "SELECT u.id, u.username FROM users u "
                                "JOIN group_members gm ON u.id = gm.user_id "
                                "JOIN groups g ON gm.group_id = g.id "
                                "WHERE g.id = ?";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la friends\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(stmt, 1, groupID);

    unsigned char* realGroupname = idToGroupname(groupID);
    int membersSize = sizeof(MEMBERS) + strlen(realGroupname) + 1;
    char* members = malloc(membersSize);
    strcat(strcat(strcpy(members, MEMBERS), realGroupname), ":");
    int ownerID = groupOwnerID(groupID);

    pthread_mutex_lock(&g_groups_mutex);
    pthread_mutex_lock(&g_groupmembers_mutex);
    pthread_mutex_lock(&g_users_mutex);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* username = sqlite3_column_text(stmt, 1);

        const char* isOwner = "";
        if (id == ownerID)
        {
            isOwner = " | OWNER";
        }
        if (id == *client_id)
        {
            username = "You";
        }

        char member[BUFFER_SIZE];
        snprintf(member, sizeof(member), "\n[%d] %s%s", id, username, isOwner);

        membersSize += strlen(member);
        members = realloc(members, membersSize);
        strcat(members, member);
    }
    sqlite3_finalize(stmt);
    
    pthread_mutex_unlock(&g_groups_mutex);
    pthread_mutex_unlock(&g_groupmembers_mutex);
    pthread_mutex_unlock(&g_users_mutex);

    free(realGroupname);

    if (rc == SQLITE_DONE)
        return members;
    else
    {
        free(members);
        printf("[server] Eroare la sqlite3_step() la friends\n");
        return SQL_ERROR;
    }
}

const char* groupsCommand(int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    int rc;
    sqlite3_stmt* stmt;
    const char* sql_select = "SELECT g.id, g.name, g.owner_id FROM groups g "
                                "JOIN group_members gm ON g.id = gm.group_id "
                                "JOIN users u ON gm.user_id = u.id "
                                "WHERE u.id = ?";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la friends\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(stmt, 1, *client_id);

    int groupsSize = sizeof(GROUPS);
    char* groups = malloc(groupsSize);
    strcpy(groups, GROUPS);

    pthread_mutex_lock(&g_groups_mutex);
    pthread_mutex_lock(&g_groupmembers_mutex);
    pthread_mutex_lock(&g_users_mutex);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* groupname = sqlite3_column_text(stmt, 1);
        int ownerID = sqlite3_column_int(stmt, 2);

        const char* isOwner = "";
        if (*client_id == ownerID)
        {
            isOwner = " | OWNER";
        }

        char group[BUFFER_SIZE];
        snprintf(group, sizeof(group), "\n[%d] %s%s", id, groupname, isOwner);

        groupsSize += strlen(group);
        groups = realloc(groups, groupsSize);
        strcat(groups, group);
    }
    sqlite3_finalize(stmt);
    
    pthread_mutex_unlock(&g_groups_mutex);
    pthread_mutex_unlock(&g_groupmembers_mutex);
    pthread_mutex_unlock(&g_users_mutex);

    if (groupsSize == sizeof(GROUPS))
    {
        groups = realloc(groups, sizeof(GROUPS_FAILED));
        strcpy(groups, GROUPS_FAILED);
    }

    if (rc == SQLITE_DONE)
        return groups;
    else
    {
        free(groups);
        printf("[server] Eroare la sqlite3_step() la friends\n");
        return SQL_ERROR;
    }
}

const char* groupchatCommand(const char* groupname, const char* message, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int rc;
    int groupID = strtol(groupname, &endptr, 10);

    if (*endptr != '\0') // userul a dat un groupname nu un id si facem rost de id dupa groupname
    {
        groupID = groupnameToId(groupname, client_id);
        if (groupID == -1)
            return KICK_GROUPNOTEXIST;
        else if (groupID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!groupExists(groupID))
            return KICK_GROUPNOTEXIST;
    }

    if (!isUserInGroup(*client_id, groupID))
        return KICK_FAILED;
    
    sqlite3_stmt* stmt;
    printf("[server] before prepare\n");
    const char* sql_insert = "INSERT into group_messages (sender_id, group_id, message) VALUES (?, ?, ?)";
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare()\n");
        return SQL_ERROR;
    }
    printf("[server] after prepare\n");

    sqlite3_bind_int(stmt, 1, *client_id);
    sqlite3_bind_int(stmt, 2, groupID);
    sqlite3_bind_text(stmt, 3, message, -1, SQLITE_STATIC);

    pthread_mutex_lock(&g_groupmessages_mutex);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_groupmessages_mutex);

    if (rc == SQLITE_DONE)
        return CHAT_SUCCESS;
    else
    {
        printf("[server] Eroare sqlite3_step() la post\n");
        return SQL_ERROR;
    }
}

const char* showgroupchatCommand(const char* groupname, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    char* endptr;
    int rc;
    int groupID = strtol(groupname, &endptr, 10);

    if (*endptr != '\0') // userul a dat un groupname nu un id si facem rost de id dupa groupname
    {
        groupID = groupnameToId(groupname, client_id);
        if (groupID == -1)
            return KICK_GROUPNOTEXIST;
        else if (groupID == -2)
            return SQL_ERROR;
    }
    else
    {
        if (!groupExists(groupID))
            return KICK_GROUPNOTEXIST;
    }

    if (!isUserInGroup(*client_id, groupID))
        return KICK_FAILED;
    
    sqlite3_stmt* stmt;
    const char* sql_select = "SELECT u.id, u.username, gm.message, gm.created_at FROM group_messages gm "
                                "JOIN groups g ON g.id = gm.group_id "
                                "JOIN users u ON u.id = gm.sender_id "
                                "WHERE g.id = ? "
                                "ORDER BY gm.created_at ASC";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la friends\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(stmt, 1, groupID);

    unsigned char* realGroupname = idToGroupname(groupID);
    int initSize;
    int chatsSize = initSize = sizeof(SHOWGROUPCHAT) + strlen(realGroupname) + 1;
    char* chats = malloc(chatsSize);
    strcpy(chats, SHOWGROUPCHAT);
    strcat(strcat(strcpy(chats, SHOWGROUPCHAT), realGroupname), ":");

    pthread_mutex_lock(&g_users_mutex);
    pthread_mutex_lock(&g_groups_mutex);
    pthread_mutex_lock(&g_groupmessages_mutex);

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* username = sqlite3_column_text(stmt, 1);
        const unsigned char* message = sqlite3_column_text(stmt, 2);
        const unsigned char* created_at = sqlite3_column_text(stmt, 3);

        char chat[BUFFER_SIZE];
        snprintf(chat, sizeof(chat), "\n(%s) [%d] %s | %s", created_at, id, username, message);

        chatsSize += strlen(chat);
        chats = realloc(chats, chatsSize);
        strcat(chats, chat);
    }
    sqlite3_finalize(stmt);
    
    pthread_mutex_unlock(&g_users_mutex);
    pthread_mutex_unlock(&g_groups_mutex);
    pthread_mutex_unlock(&g_groupmessages_mutex);

    free(realGroupname);

    if (chatsSize == initSize)
    {
        chats = realloc(chats, sizeof(CHATS_FAILED));
        strcpy(chats, CHATS_FAILED);
    }

    if (rc == SQLITE_DONE)
        return chats;
    else
    {
        free(chats);
        printf("[server] Eroare la sqlite3_step() la friends\n");
        return SQL_ERROR;
    }
}

const char* privacyCommand(const char* type, int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;

    int int_type = 0;
    if (strcmp(type, PRIVACY_PUBLIC) == 0)
        int_type = public;
    else if (strcmp(type, PRIVACY_PRIVATE) == 0)
        int_type = private;
    else
        return PRIVACY_INVALID;

    int rc;
    sqlite3_stmt* stmt;
    const char* sql_update = "UPDATE users SET privacy = ? WHERE id = ?";
    rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la friendtype\n");
        return SQL_ERROR;
    }

    sqlite3_bind_int(stmt, 1, int_type);
    sqlite3_bind_int(stmt, 2, *client_id);

    pthread_mutex_lock(&g_friends_mutex);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    pthread_mutex_unlock(&g_friends_mutex);

    if (rc == SQLITE_DONE)
        return PRIVACY_SUCCESS;
    else
    {
        printf("[server] Eroare la sqlite3_step() la friendtype\n");
        return SQL_ERROR;
    }
}

const char* userCommand(int* client_id)
{
    if (*client_id == LOGGED_OUT)
        return NOT_LOGGED;
    
    int rc;
    sqlite3_stmt* stmt;
    const char* sql_select = "SELECT username, " // usernameul
                                    "COUNT(" // numarul de prieteni
                                        "SELECT 1 FROM users u "
                                        "JOIN friends f ON (f.user_id = u.id OR f.friend_id = u.id) "
                                        "WHERE u.id = ? AND f.status = 1 "
                                        "),"
                                    "COUNT(" // numarul de postari
                                        "SELECT 1 FROM users u "
                                        "JOIN posts p ON p.user_id = u.id "
                                        "WHERE u.id = ?"
                                        "),"
                                    "COUNT(" // numarul de grupuri in care este
                                        "SELECT 1 FROM users u "
                                        "JOIN group_members gm ON gm.user_id = u.id "
                                        "JOIN groups g ON g.id = gm.group_id "
                                        "WHERE u.id = ?"
                                        "),"
                                    "COUNT(" // numarul de dmuri trimise
                                        "SELECT 1 FROM users u "
                                        "JOIN messages m ON m.sender_id = u.id "
                                        "WHERE u.id = ?"
                                        "),"
                                    "COUNT(" // numarul de dmuri primite
                                        "SELECT 1 FROM users u "
                                        "JOIN messages m ON m.receiver_id = u.id "
                                        "WHERE u.id = ?"
                                        "),"
                                    "COUNT(" // numarul de mesaje trimise in grupuri
                                        "SELECT 1 FROM users u "
                                        "JOIN group_messages gm ON gm.sender_id = u.id "
                                        "WHERE u.id = ?"
                                        "),"
                                    "COUNT(" // numarul de mesaje primite in grupuri
                                        "SELECT 1 FROM users u "
                                        "JOIN group_members gm ON gm.user_id = u.id "
                                        "JOIN groups g ON g.id = gm.group_id "
                                        "JOIN group_messages m ON m.group_id = g.id "
                                        "WHERE u.id = ? AND m.sender_id != u.id"
                                        ") "
                                    "FROM users WHERE id = ?";
    rc = sqlite3_prepare_v2(db, sql_select, -1, &stmt, 0);
    if (rc != SQLITE_OK)
    {
        printf("[server] Eroare sql_prepare() la user\n");
        return SQL_ERROR;
    }
    sqlite3_bind_int(stmt, 1, *client_id);
    sqlite3_bind_int(stmt, 2, *client_id);
    sqlite3_bind_int(stmt, 3, *client_id);
    sqlite3_bind_int(stmt, 4, *client_id);
    sqlite3_bind_int(stmt, 5, *client_id);
    sqlite3_bind_int(stmt, 6, *client_id);
    sqlite3_bind_int(stmt, 7, *client_id);

    char* output;
    
    pthread_mutex_lock(&g_users_mutex);
    pthread_mutex_lock(&g_friends_mutex);
    pthread_mutex_lock(&g_posts_mutex);
    pthread_mutex_lock(&g_chats_mutex);
    pthread_mutex_lock(&g_groups_mutex);
    pthread_mutex_lock(&g_groupmembers_mutex);
    pthread_mutex_lock(&g_groupmessages_mutex);

    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        const unsigned char* username = sqlite3_column_text(stmt, 0);
        int friendCount = sqlite3_column_int(stmt, 1);
        int postCount = sqlite3_column_int(stmt, 2);
        int groupCount = sqlite3_column_int(stmt, 3);
        int sentdmCount = sqlite3_column_int(stmt, 4);
        int receiveddmCount = sqlite3_column_int(stmt, 5);
        int sentgroupdmCount = sqlite3_column_int(stmt, 6);
        int receivedgroupdmCount = sqlite3_column_int(stmt, 7);

        char final[BUFFER_SIZE];
        snprintf(final, sizeof(final), "\nUsername:\t%s\n"
                                        "Number of friends:\t%d\n"
                                        "Number of posts:\t%d\n"
                                        "Number of groups joined:\t%d\n"
                                        "Number of DMs sent:\t%d\n"
                                        "Number of DMs received:\t%d\n"
                                        "Number of Group DMs sent:\t%d\n"
                                        "Number of Group DMs received:\t%d",
                                        username, friendCount, postCount,
                                        groupCount, sentdmCount, receiveddmCount,
                                        sentgroupdmCount, receivedgroupdmCount);

        output = malloc(strlen(final)) + 1;
        strcpy(output, final);
    }
    sqlite3_finalize(stmt);
    
    pthread_mutex_unlock(&g_users_mutex);
    pthread_mutex_unlock(&g_friends_mutex);
    pthread_mutex_unlock(&g_posts_mutex);
    pthread_mutex_unlock(&g_chats_mutex);
    pthread_mutex_unlock(&g_groups_mutex);
    pthread_mutex_unlock(&g_groupmembers_mutex);
    pthread_mutex_unlock(&g_groupmessages_mutex);

    if (rc == SQLITE_DONE)
        return output;
    else
    {
        free(output);
        printf("[server] Eroare la sqlite3_step() la friends\n");
        return SQL_ERROR;
    }
}

const char* banCommand(int* client_id)
{
    pthread_mutex_lock(&g_users_mutex);

    if (*client_id != LOGGED_OUT)
    {
        pthread_mutex_unlock(&g_users_mutex);
        return generateOutput("BAN_COMMAND");
    }
    else
    {
        pthread_mutex_unlock(&g_users_mutex);
        return NOT_LOGGED;
    }
}