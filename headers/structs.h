#pragma once
#include <sqlite3.h>
#define usernameLength 32
#define passwordLength 32

enum userType
{
    regular_user, admin_user
};

enum privacyType
{
    public, private
};

enum friendType
{
    regular_friend, close_friend
};

sqlite3 *db;