#define strtok_r __strtok_r

#define OK_RECEIVED "TO BE... - OK RECEIVED THE COMMAND: "
#define SQL_ERROR "ERROR WHEN ACCESSING DATABASE"

#define NO_ARGUMENTS "NO ARGUMENTS. TRY HELP"
#define LESS_ARGUMENTS "LESS ARGUMENTS THAN EXPECTED. TRY HELP"
#define MORE_ARGUMENTS "MORE ARGUMENTS THAN EXPECTED. TRY HELP"
#define EMPTY_INPUT "EMPTY INPUT. TRY HELP"
#define UNKNOWN_COMMAND "UNKNOWN COMMAND. TRY HELP"

#define REGISTER_COMMAND "REGISTER"
#define REGISTER_ARGC 3
#define REGISTER_SUCCESS "REGISTERED SUCCESSFULLY"
#define REGISTER_FAILED "ALREADY REGISTERED"
#define REGULAR_USER "REGULAR"
#define ADMIN_USER "ADMIN"
#define INVALID_TYPE "INVALID USER TYPE (REGULAR/ADMIN)"
#define USERNAME_TOO_LONG "USERNAME TOO LONG (>= 31 characters)"
#define PASSWORD_TOO_LONG "PASSWORD TOO LONG (>= 31 characters)"

#define LOGIN_COMMAND "LOGIN"
#define LOGIN_ARGC 2
#define LOGIN_SUCCESS "LOGGED IN SUCCESSFULLY"
#define LOGIN_FAILED "WRONG USERNAME/PASSWORD. TRY AGAIN"
#define LOGIN_ALREADY "ALREADY LOGGED IN"

#define LOGOUT_COMMAND "LOGOUT"
#define LOGOUT_ARGC 0
#define LOGOUT_SUCCESS "LOGGED OUT SUCCESSFULLY"
#define NOT_LOGGED "NOT LOGGED IN"

#define ADD_COMMAND "ADD"
#define ADD_ARGC 1
#define ADD_SUCCESS "ADDED SUCCESSFULLY"
#define ADD_ALREADY_FRIENDS "ALREADY FRIENDS"
#define ADD_YOU "REQUEST ALREADY SENT BY YOU"
#define ADD_OTHER "YOU ALREADY HAVE A REQUEST FROM THIS USER. USE ACCEPT <user_id/username>"
#define ADD_FAILED "USER DOESN'T EXIST"
#define ADD_SELF "CAN'T ADD SELF"
#define REGULAR_FRIEND REGULAR_USER
#define CLOSE_FRIEND "CLOSE"

#define REQUEST_ARGC 1
#define REQUEST_ACCEPT_SUCCESS "REQUEST ACCEPTED"
#define REQUEST_DECLINE_SUCCESS "REQUEST DECLINED/CANCELLED"
#define REQUEST_FAILED "NO REQUEST FROM USER"
#define REQUEST_SELF "CAN'T ACCEPT/DECLINE SELF"
#define ACCEPT_COMMAND "ACCEPT"
#define DECLINE_COMMAND "DECLINE"

#define REQUESTS_COMMAND "REQUESTS"
#define REQUESTS_ARGC 0
#define REQUESTS_NONE "NO INCOMING/OUTGOING REQUESTS"
#define REQUESTS_NONE_IN "\nNO INCOMING REQUESTS"
#define REQUESTS_NONE_OUT "\nNO OUTGOING REQUESTS"
#define REQUESTS_IN "\nINCOMING REQUESTS:"
#define REQUESTS_OUT "\nOUTGOING REQUESTS:"

#define REMOVE_COMMAND "REMOVE"
#define REMOVE_ARGC 1
#define REMOVE_SUCCESS "FRIEND REMOVED SUCESSFULLY"
#define REMOVE_FAILED "NOT FRIENDS"
#define REMOVE_SELF "CAN'T REMOVE SELF"

#define FRIENDS_COMMAND "FRIENDS"
#define FRIENDS_ARGC 0
#define FRIENDS_FAILED "NO FRIENDS ADDED"
#define FRIENDS "\nFRIENDS:"

#define FRIENDTYPE_COMMAND "FRIENDTYPE" //
#define FRIENDTYPE_ARGC 2
#define FRIENDTYPE_SUCCESS "FRIEND TYPE CHANGED"
#define FRIENDTYPE_FAILED "NOT FRIENDS"
#define FRIENDTYPE_SELF "CAN'T CHANGE SELF FRIENDTYPE"
#define INVALID_FRIEND_TYPE "INVALID FRIEND TYPE (REGULAR/CLOSE)"

#define POST_COMMAND "POST" //
#define POST_ARGC 2
#define POST_SUCCESS "POSTED SUCCESSFULLY"
#define POST_FAILED "POST FAILED"

#define VIEW_COMMAND "VIEW" //
#define VIEW_ARGC 0

#define CHAT_COMMAND "CHAT" //
#define CHAT_ARGC 2

#define CHATS_COMMAND "CHATS" //
#define CHATS_ARGC 0

#define SHOWCHAT_COMMAND "SHOWCHAT" //
#define SHOWCHAT_ARGC 1

#define GROUPCHAT_COMMAND "GROUPCHAT" //
#define GROUPCHAT_ARGC 2

#define GROUPCHATS_COMMAND "GROUPCHATS" //
#define GROUPCHATS_ARGC 0

#define SHOWGROUPCHAT_COMMAND "SHOWGROUPCHAT" //
#define SHOWGROUPCHAT_ARGC 1

#define PRIVACY_COMMAND "PRIVACY" //
#define PRIVACY_ARGC 1

#define BAN_COMMAND "BAN" //
#define BAN_ARGC 2

#define HELP_COMMAND "HELP"
#define HELP_ARGC 0
#define HELP \
"\n\
    REGISTER <username> <password> REGULAR/ADMIN - Registers an account with the given permissions.\n\
    LOGIN <username> <password> - Logs into an account.\n\
    LOGOUT - Logs out of account.\n\
    ADD <user_id/username> - Sends a request to the user that matches the given user_id/username (default: regular).\n\
    ACCEPT <user_id/username> - Accepts the request received from user_id/username.\n\
    DECLINE <user_id/username> - Declines/Cancels the request received from/sent to user_id/username.\n\
    REQUESTS - Shows all friend requests.\n\
    REMOVE <user_id/username> - Removes the user that matches the given user_id/username.\n\
    FRIENDS - Shows all friends of the logged in user.\n\
    FRIENDTYPE <user_id/username> REGULAR/CLOSE - Changes provided user's friend type to regular/close\n\
    POST PUBLIC/REGULAR/CLOSE <message> - Posts the given message on your account with the given privacy.\n\
    VIEW - Shows all the posts available.\n\
    CHAT <user_id/username> <message> - Sends the given message to the given user_id/username.\n\
    CHATS - Shows the available chats (minimum 1 message sent/received).\n\
    SHOWCHAT <used_id/username> - Shows the chat with the given user_id/username.\n\
    CREATEGROUP <groupname> - Creates a group with the given name.\n\
    INVITE <user_id/username> <group_id/groupname> - Invites the given user_id/username to the given group_id/groupname.\n\
    INVITES - Shows all group invites.\n\
    KICK <user_id/username> <group_id/groupname> - Kicks the given user_id/username from the given group_id/groupname.\n\
    ACCEPTGROUP <group_id/groupname> - Accepts the invite to the given group_id/username.\n\
    GROUPCHAT <group_id/groupname> <message> - Sends the given message to the given group_id/groupname.\n\
    GROUPCHATS - Shows the available group chats (minimum 1 message sent/received).\n\
    SHOWGROUPCHAT <group_id/groupname> - Shows the chat of the given group_id/groupname.\n\
    PRIVACY PUBLIC/PRIVATE - Sets the profile's and posts' privacy to the given privacy.\n\
    BAN <user_id/username> <reason> - Bans the user that matches the given user_id/username. Only for admins!\
"

#define LOGGED_OUT -1