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
#define REGISTER_SUCCESS "REGISTERED SUCCESSFULLY. USE LOGIN <username> <password> TO LOG IN."
#define REGISTER_FAILED "ALREADY REGISTERED. USE LOGIN <username> <password> TO LOG IN."
#define REGULAR_USER "REGULAR"
#define ADMIN_USER "ADMIN"
#define INVALID_TYPE "INVALID USER TYPE (REGULAR/ADMIN)"
#define USERNAME_TOO_LONG "USERNAME TOO LONG"
#define PASSWORD_TOO_LONG "PASSWORD TOO LONG"

#define LOGIN_COMMAND "LOGIN"
#define LOGIN_ARGC 2
#define LOGIN_SUCCESS "LOGGED IN SUCCESSFULLY"
#define LOGIN_FAILED "WRONG USERNAME/PASSWORD. TRY AGAIN"
#define LOGIN_ALREADY "ALREADY LOGGED IN. USE LOGOUT TO LOG OUT."

#define LOGOUT_COMMAND "LOGOUT"
#define LOGOUT_ARGC 0
#define LOGOUT_SUCCESS "LOGGED OUT SUCCESSFULLY"
#define NOT_LOGGED "NOT LOGGED IN"

#define ADD_COMMAND "ADD"
#define ADD_ARGC 1
#define ADD_SUCCESS "ADDED SUCCESSFULLY. Use REQUESTS to see friend requests."
#define ADD_ALREADY_FRIENDS "ALREADY FRIENDS. Use REQUESTS to see friend requests."
#define ADD_YOU "REQUEST ALREADY SENT BY YOU"
#define ADD_OTHER "YOU ALREADY HAVE A REQUEST FROM THIS USER. Use ACCEPT <user_id/username> to accept the friend request."
#define ADD_FAILED "USER DOESN'T EXIST"
#define ADD_SELF "CAN'T ADD SELF"
#define REGULAR_FRIEND REGULAR_USER
#define CLOSE_FRIEND "CLOSE"

#define REQUEST_ARGC 1
#define REQUEST_ACCEPT_SUCCESS "REQUEST ACCEPTED. Use FRIENDS to see friends."
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
#define REQUESTS_SUFFIX "\nUse ACCEPT/DECLINE <user_id/username> to accept/decline[cancel] the given user_id/username's request."

#define REMOVE_COMMAND "REMOVE"
#define REMOVE_ARGC 1
#define REMOVE_SUCCESS "FRIEND REMOVED SUCCESSFULLY"
#define REMOVE_FAILED "NOT FRIENDS"
#define REMOVE_SELF "CAN'T REMOVE SELF"

#define FRIENDS_COMMAND "FRIENDS"
#define FRIENDS_ARGC 0
#define FRIENDS_FAILED "NO FRIENDS ADDED"
#define FRIENDS "\nFRIENDS:"
#define FRIENDS_SUFFIX "\nUse ADD/REMOVE <user_id/username> to add/remove the person with the given user_id/username."

#define FRIENDTYPE_COMMAND "FRIENDTYPE"
#define FRIENDTYPE_ARGC 2
#define FRIENDTYPE_SUCCESS "FRIEND TYPE CHANGED"
#define FRIENDTYPE_FAILED "NOT FRIENDS"
#define FRIENDTYPE_SELF "CAN'T CHANGE SELF FRIENDTYPE"
#define INVALID_FRIEND_TYPE "INVALID FRIEND TYPE (REGULAR/CLOSE)"

#define POST_COMMAND "POST"
#define POST_ARGC 2
#define POST_SUCCESS "POSTED SUCCESSFULLY. Use POSTS to see available posts."
#define POST_FAILED "POST FAILED"
#define POST_PUBLIC "PUBLIC"
#define POST_REGULAR REGULAR_FRIEND
#define POST_CLOSE CLOSE_FRIEND
#define INVALID_POST_TYPE "INVALID POST TYPE (PUBLIC/REGULAR/CLOSE)"

#define POSTS_COMMAND "POSTS"
#define POSTS_ARGC 0
#define POSTS "\nPOSTS:"
#define POSTS_FAILED "NO POSTS AVAILABLE"
#define POSTS_SUFFIX "\nUse POST PUBLIC/REGULAR/CLOSE <message> to post a message with the given privacy."

#define CHAT_COMMAND "CHAT"
#define CHAT_ARGC 2
#define CHAT_SELF "CAN'T MESSAGE SELF"
#define CHAT_SUCCESS "CHAT SENT. Use CHATS to see chats."

#define CHATS_COMMAND "CHATS"
#define CHATS_ARGC 0
#define CHATS "\nAVAILABLE CHATS:"
#define CHATS_FAILED "NO CHATS AVAILABLE"
#define CHATS_SUFFIX "\nUse SHOWCHAT <user_id/username> to see the chat with the given user_id/username."

#define SHOWCHAT_COMMAND "SHOWCHAT"
#define SHOWCHAT_ARGC 1
#define SHOWCHAT_SELF CHAT_SELF
#define SHOWCHAT "\nCHAT WITH "
#define SHOWCHAT_SUFFIX "\nUse CHAT <user_id/username> <message> to send a message to the given user_id/username."

#define CREATEGROUP_COMMAND "CREATEGROUP"
#define CREATEGROUP_ARGC 1
#define GROUPNAME_TOO_LONG "GROUP NAME TOO LONG"
#define CREATEGROUP_SUCCESS "GROUP CREATED. Use GROUPS to see joined groups."
#define CREATEGROUP_FAILED "GROUP NAME ALREADY TAKEN"

#define INVITE_COMMAND "INVITE"
#define INVITE_ARGC 2
#define INVITE_SELF "CAN'T INVITE SELF"
#define INVITE_SUCCESS "USER ADDED TO GROUP. Use MEMBERS <group_id/groupname> to see group members."
#define INVITE_FAILED "YOU ARE NOT IN THE GROUP"
#define INVITE_GROUPNOTEXIST "GROUP DOESN'T EXIST"
#define INVITE_ALREADY "USER ALREADY IN GROUP"

#define KICK_COMMAND "KICK"
#define KICK_ARGC 2
#define KICK_SELF "CAN'T KICK SELF. USE LEAVE"
#define KICK_SUCCESS "USER KICKED. Use MEMBERS <group_id/groupname> to see group members."
#define KICK_FAILED INVITE_FAILED
#define KICK_NOTOWNER "YOU ARE NOT THE OWNER OF THE GROUP"
#define KICK_GROUPNOTEXIST INVITE_GROUPNOTEXIST
#define KICK_ALREADY "USER IS NOT IN GROUP"

#define LEAVE_COMMAND "LEAVE"
#define LEAVE_ARGC 1
#define LEAVE_SUCCESS "GROUP LEFT. Use GROUPS to see joined groups."
#define LEAVE_FAILED INVITE_FAILED

#define MEMBERS_COMMAND "MEMBERS"
#define MEMBERS_ARGC 1
#define MEMBERS "\nMEMBERS OF "

#define GROUPS_COMMAND "GROUPS"
#define GROUPS_ARGC 0
#define GROUPS "\nJOINED GROUPS:"
#define GROUPS_FAILED "NO GROUPS AVAILABLE"
#define GROUPS_SUFFIX "\nUse CREATEGROUP <groupname> to create a group with the given groupname."\
                        "\nUse SHOWGROUPCHAT <group_id/groupname> to see a joined group's chat."

#define GROUPCHAT_COMMAND "GROUPCHAT"
#define GROUPCHAT_ARGC 2
#define GROUPCHAT_SUCCESS "CHAT SENT. Use SHOWGROUPCHAT <group_id/groupname> to see a joined group's chat."

#define SHOWGROUPCHAT_COMMAND "SHOWGROUPCHAT"
#define SHOWGROUPCHAT_ARGC 1
#define SHOWGROUPCHAT "\nCHATS IN "
#define SHOWGROUPCHAT_SUFFIX "\nUse GROUPCHAT <group_id/groupname> <message> to send a message to the a joined group."

#define PRIVACY_COMMAND "PRIVACY"
#define PRIVACY_ARGC 1
#define PRIVACY_PUBLIC "PUBLIC"
#define PRIVACY_PRIVATE "PRIVATE"
#define PRIVACY_INVALID "INVALID PRIVACY TYPE (PUBLIC/PRIVATE)"
#define PRIVACY_SUCCESS "PRIVACY CHANGED"

#define USER_COMMAND "USER"
#define USER_ARGC 0

#define BAN_COMMAND "BAN"
#define BAN_ARGC 1
#define BAN_NOTADMIN "YOU ARE NOT AN ADMIN"
#define BAN_SUCCESS "USER BANNED"

#define SEARCH_COMMAND "SEARCH"
#define SEARCH_ARGC 1
#define SEARCH "FOUND USERS FOR "
#define SEARCH_FAILED "NO USERS FOUND"

#define HELP_COMMAND "HELP"
#define HELP_ARGC 0
#define HELP \
"\n\
    SEARCH <username> - Shows the users whose username contain the given username. Any amount of '%' as username gives all users, any amount of '_' as username gives users with at least the amount of _'s.\n\
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
    POSTS - Shows all the posts available.\n\
    CHAT <user_id/username> <message> - Sends the given message to the given user_id/username.\n\
    CHATS - Shows the available chats (minimum 1 message sent/received).\n\
    SHOWCHAT <user_id/username> - Shows the chat with the given user_id/username.\n\
    CREATEGROUP <groupname> - Creates a group with the given name.\n\
    INVITE <user_id/username> <group_id/groupname> - Adds the given user_id/username to the given group_id/groupname.\n\
    KICK <user_id/username> <group_id/groupname> - Kicks the given user_id/username from the given group_id/groupname. Must be the owner!\n\
    LEAVE <group_id/groupname> - Leaves the group with the given group_id/groupname.\n\
    MEMBERS <group_id/groupname> - Shows the members of the given group_id/groupname.\n\
    GROUPS - Shows the groups you are a member of.\n\
    GROUPCHAT <group_id/groupname> <message> - Sends the given message to the given group_id/groupname.\n\
    SHOWGROUPCHAT <group_id/groupname> - Shows the chat of the given group_id/groupname.\n\
    PRIVACY PUBLIC/PRIVATE - Sets the profile's privacy to the given privacy.\n\
    USER - Shows info about logged user.\n\
    BAN <user_id/username> - Bans (Deletes) the user's account that matches the given user_id/username. Only for admins!\
"

#define LOGGED_OUT -1