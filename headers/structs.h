#pragma once
#include <sqlite3.h>
#define usernameLength 64
#define passwordLength 64
#define groupnameLength usernameLength

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

enum postType
{
    public_post, regular_post, close_post
};

sqlite3 *db;