/*
 * Copyright (C) 2016, 2018 Daniel Martin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GNU_SOCIAL_H
#define GNU_SOCIAL_H

#include <string.h>
#include <stdio.h>

#include "constants.h"

struct gnusocial_account_info {
    char name[MAX_ACCOUNT_NAME];
    char screen_name[MAX_SCREEN_NAME];
    char location[MAX_LOCATION];
    char description[MAX_DESCRIPTION];
    char url[MAX_URL];
    int followers;
    int friends;
    int statuses;
    char profile_image_url[MAX_URL];
    char profile_image_url_profile_size[MAX_URL];
};

typedef struct gnusocial_account_info gnusocial_account_info_t;

struct gnusocial_account {
    char protocol[GNUSOCIAL_ACCOUNT_PROTOCOL_SIZE];
    char user[GNUSOCIAL_ACCOUNT_USERNAME_SIZE];
    char password[GNUSOCIAL_ACCOUNT_PASSWORD_SIZE];
    char server[GNUSOCIAL_ACCOUNT_SERVER_SIZE];
    char socks_proxy[GNUSOCIAL_ACCOUNT_PROXY_SIZE];
};

typedef struct gnusocial_account gnusocial_account_t;

struct gnusocial_status {
    char text[1024];
    int id;
    char author_screen_name[64];
    int in_reply_to_id;
    char in_reply_to_user[64];
    char date[64];
};

typedef struct gnusocial_status gnusocial_status_t;

struct gnusocial_group_info {
    int id;
    char url[MAX_URL];
    char nickname[MAX_GROUP_NICKNAME];
    char fullname[MAX_GROUP_FULLNAME];
    /* ZERO if not, Non-ZERO if true */
    int member;
    int admins;
    int members;
    char description[MAX_DESCRIPTION];
};

typedef struct gnusocial_group_info gnusocial_group_info_t;

struct gnusocial_session {
	gnusocial_account_t *account;
	gnusocial_status_t *status;
	unsigned int n_status;
	gnusocial_group_info_t *groups;
	unsigned int n_groups;
	gnusocial_account_info_t *accounts;
	unsigned int n_accounts;
	char *errormsg;
	char *xml;
	char *source;
};

typedef struct gnusocial_session gnusocial_session_t;

/**
 * @brief Allocates enough space for a gnusocial_session_t
 * @return A pointer to a gnusocial_session_t if succeed and NULL if failed
 */

gnusocial_session_t *gnusocial_session_create(void);

/**
 * @brief Frees a gnusocial_session_t previously created
 * @param session A gnusocial_session_t to be freed
 */

void gnusocial_session_free(gnusocial_session_t *session);

/**
 * @brief Returns the full xml file got from the API
 * @param session gnusocial_session_t of which you want to get the XML file
 * @return A pointer to the XML file content
 */

char *gnusocial_session_xml(gnusocial_session_t *session);

/**
 * @brief Returns an exact cURL or GNUsocial API error message
 * @param session gnusocial_session_t where the error took place
 * @return A pointer to the error message (string)
 */

char *gnusocial_session_error_reason(gnusocial_session_t *session);

/**
 * @brief Resets the gnusocial session data from a request, usually to make a new request
 * @param session gnusocial_session_t of which we want to reset the request data
 */

void gnusocial_session_reset(gnusocial_session_t *session);

/**
 * @brief Sets a given program name (string) as source for post requests to the GS API
 * @param session Session structure to set the source in
 * @param source String to be set (copied) as source in session
 */

void gnusocial_session_set_source(gnusocial_session_t *session, const char *source);

/**
 * @brief Frees source member of structure session (if it does not point to NULL)
 * @param session Session structure to remove source
 */

void gnusocial_session_remove_source(gnusocial_session_t *session);

/**
 * @brief Post a new notice as the authenticating user
 * @param session A session structure to authenticate the user into the server
 * @param msg String that contains the notice content to post
 */

int gnusocial_post_status(gnusocial_session_t *session, char *msg);

/**
 * @brief Favorites the status specified in the ID parameter as the authenticating user
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The notice ID to favorite
 * @return zero if succeed, negative integer if error
 */

int gnusocial_favorite_status(gnusocial_session_t *session, int id);

/**
 * @brief Un-favorites the status specified in the ID parameter as the authenticating user
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The notice ID to un-favorite
 * @return zero if succeed, a negative integer if error
 */

int gnusocial_unfavorite_status(gnusocial_session_t *session, int id);

/**
 * @brief Prints a single notice, specified by the id parameter
 * @param session A session structure to authenticate the user into the server
 * @param id The notice ID to print
 * @return zero if succeed, a negative integer if error
 */

int gnusocial_search_status(gnusocial_session_t *session, int id);

/**
 * @brief Destroys the notice specified by the required ID parameter
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The notice ID to destroy
 * @return zero if suceed, negative integer if error
 */

int gnusocial_delete_status(gnusocial_session_t *session, int id);

/**
 * @brief Post a new notice in reply specified by the required ID parameter
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The notice ID to reply
 * @return zero if succeed, a negative integer if error
 */

int gnusocial_reply_status(gnusocial_session_t *session, int id, char *msg);

/**
 * @brief Read a number of status from 'timeline'
 * @param session A session structure to authenticate the user into the server
 * @param timeline The timeline path to read
 * @param n_status The number of status to read from timeline
 * @return zero if succeed, negative integer if not (error)
 */

int gnusocial_read_timeline(gnusocial_session_t *session, char *timeline, int n_status);

/**
 * @brief Repeats a notice espeficied by its ID
 * @param session A session structure to authenticate the user into the server
 * @param id The notice ID to repeat
 * @param code Should be 1 to work correctly
 */

int gnusocial_repeat_status(gnusocial_session_t *session, int id, int code);

/**
 * @brief Gets user information especified by "source"
 * @param session Session structure to authenticate the user into the server
 * @param source The user to get the information. Can be used the user ID or user SCREEN_NAME
 * Example: source="&id=1" source="&screen_name=4dmin"
 * @return zero if succeed, negative integer if error
 */

int gnusocial_get_user_info(gnusocial_session_t *session, char *source);

/**
 * @brief Checks if user credentials are correct
 * @param session Session structure that contains account credentials
 */

int gnusocial_verify_account(gnusocial_session_t *session);

/**
 * @brief Gets group information espeficied by its ID
 * @param session Session structure to authenticate the user into the server
 * @param id The group ID to get the info
 * @return ZERO if succeed, a negative integer if error
 */

int gnusocial_get_group_info(gnusocial_session_t *session, int id);

/**
 * @brief Join a group espeficied by its ID
 * @param session Session structure to authenticate the user into the server
 * @param id The group ID to join
 */

int gnusocial_join_group(gnusocial_session_t *session, int id);

/**
 * @brief Leave a group especified by its ID
 * @param session Session structure to authenticate the user into the server
 * @param id The group ID to leave
 */

int gnusocial_leave_group(gnusocial_session_t *session, int id);

/**
 * @brief Gets little information of a number of groups especified by a 'n_groups'
 * @param session Session structure to authenticate the user into the server
 * @param n_groups The number of groups to be readed
 * @param timeline The timeline (path to XML file) to make the request 
 * @return ZERO if succeed, a negative integer if error
 */

int gnusocial_list_groups(gnusocial_session_t *session, int n_groups, char *timeline);

/**
 * @brief Start to follow a user especified by its screen_name
 * @param session Session structure to authenticate the user into the server
 * @param screen_name The user screen_name to follow
 * @return zero if succeed, negative integer if error
 */

int gnusocial_follow_user(gnusocial_session_t *session, char *screen_name);

/**
 * @brief Stop to follow a user espeficied by its screen_name
 * @param session Session structure to authenticate the user into the server
 * @param screen_name The user to unfollow
 * @return zero if succeed, negative integer if error
 */

int gnusocial_unfollow_user(gnusocial_session_t *session, char *screen_name);

/**
 * @brief Prints a list of users from a especified list
 * @param source The source list to read the users
 * @param n_users The number of users to read
 */

/*void gs_print_users_array_info(gnusocial_account_t account,
                               char *source, int n_users);*/

/**
 * @brief Sends a formatted content to the server API
 * @param session Session structure that contains account information and where
 * the data requested to the API is stored
 * @param send The formatted content to be sent to the server API
 * @param xml_doc The XML file at GNUsocial API to where the request will be made
 * Documentation: https://gnusocial.net/doc/twitterapi
 */

int gnusocial_api_request(gnusocial_session_t *session, char *send, char *xml_doc);

/**
 * @brief Get the number of groups a user is member of
 * @param session Session structure
 * @param username Name of the user of whom we want to know the number of groups he is
 * member of
 * @return The number of groups of which "username" is member, or a negative error code
 */

int gnusocial_get_number_of_groups(gnusocial_session_t *session, const char *username);

/**
 * @brief Populates a session structure with login details
 * @param session Session structure
 * @param protocol https
 * @param user Username
 * @param server Domain name of the gnusocial server
 * @param password Login password
 * @param socks_proxy Optional socks proxy address:port
 * @return ZERO if succeed, negative integer if an error found
 */
int gnusocial_set_account(gnusocial_session_t *session, char *protocol, 
	char *user, char *server, char *password, char *socks_proxy);

/**
 * @brief Export followed users to a text file
 * @param session Session structure
 * @param filename File to be created
 * @return zero if succeed, negative integer if error
 */

int gnusocial_export_users(gnusocial_session_t *session, char *filename);

/**
 * @brief Import users from a text file
 * @param session Session structure
 * @param filename File containing exported user follows
 */

void gnusocial_import_users(gnusocial_session_t *session, char *filename);

/**
 * @brief saves the avatar image for the given local user
 * @param session Session structure
 * @param username username on the local server
 * @param avatar_filename File to save
 * @returns zero on success
 */
int gnusocial_get_user_avatar(gnusocial_session_t *session, char *username,
                       char *avatar_filename);

/**
 * @brief saves the avatar image for a user being followed
 * @param session Session structure
 * @param username username being followed
 * @param avatar_filename File to save
 * @returns zero on success
 */
int gnusocial_get_follow_avatar(gnusocial_session_t *session, char *username,
                         char *avatar_filename);

/**
 * @brief Returns a general error string that matches a given error code
 * @param error_code Error code to find the error string
 * @return A pointer to a const-char error string
 */

const char *gnusocial_get_error(int error_code);

#endif
