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

struct gnusocial_little_group_info {
    int id;
    char nickname[MAX_GROUP_NICKNAME];
    char description[MAX_DESCRIPTION];
};

typedef struct gnusocial_little_group_info gnusocial_little_group_info_t;

struct gnusocial_session {
	gnusocial_account_t *account;
	gnusocial_status_t *status;
	unsigned int n_status;
	char *errormsg;
	char *xml;
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
 * @brief Returns a curl or GNUsocial API error message
 * @param session gnusocial_session_t where the error took place
 * @return A pointer to the error message (string)
 */

char *gnusocial_session_error(gnusocial_session_t *session);

/**
 * @brief Resets the gnusocial session data from a request, usually to make a new request
 * @param session gnusocial_session_t of which we want to reset the request data
 */

void gnusocial_session_reset(gnusocial_session_t *session);

/**
 * @brief Downloads the user information from the GnuSocial server
 * @param account A gss_account structure to send the request to the GnuSocial server
 * @param result An intenger to store if there is an error or not. ZERO if not, Non-ZERO if yes
 * @return The user information as an account_info structure
 */

gnusocial_account_info_t gs_get_my_account_info(gnusocial_account_t account, int *result);

/**
 * @brief Post a new notice as the authenticating user
 * @param account A gss_account structure to authenticate the user into the server
 * @param msg String that contains the notice content to post
 */

void gnusocial_post_status(gnusocial_account_t account, char *msg);

/**
 * @brief Favorites the status specified in the ID parameter as the authenticating user
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The notice ID to favorite
 */

void gnusocial_favorite_status(gnusocial_account_t account, int id);

/**
 * @brief Un-favorites the status specified in the ID parameter as the authenticating user
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The notice ID to un-favorite
 */

void gnusocial_unfavorite_status(gnusocial_account_t account, int id);

/**
 * @brief Prints a single notice, specified by the id parameter
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The notice ID to print
 * @param result An intenger to store Non-ZERO if there is an error, ZERO if not
 * @return An status structure with the status information
 */

gnusocial_status_t gnusocial_search_status(gnusocial_account_t account, int id, int *result);

/**
 * @brief Destroys the notice specified by the required ID parameter
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The notice ID to destroy
 */

void gnusocial_delete_status(gnusocial_account_t account, int id);

/**
 * @brief Post a new notice in reply specified by the required ID parameter
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The notice ID to reply
 */

void gnusocial_reply_status(gnusocial_account_t account, int id, char *msg);

/**
 * @brief Read a number of status from 'timeline'
 * @param account A gss_account structure to authenticate the user into the server
 * @param timeline The timeline path to read
 * @param n_status The number of status to read from timeline
 * @return An array of status
 */

gnusocial_status_t *gnusocial_read_timeline(gnusocial_account_t account, char *timeline, int n_status);

/**
 * @brief Repeats a notice espeficied by its ID
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The notice ID to repeat
 * @param code Should be 1 to work correctly
 */

void gnusocial_repeat_status(gnusocial_account_t account, int id, int code);

/**
 * @brief Gets user information especified by "source"
 * @param account A gss_account structure to authenticate the user into the server
 * @param source The user to get the information. Can be used the user ID or user SCREEN_NAME
 * Example: source="&id=1" source="&screen_name=4dmin"
 * @return A account_info structure with all the info
 */

gnusocial_account_info_t gs_get_user_info(gnusocial_account_t account, char *source);

/**
 * @brief Checks if user credentials are correct
 * @param A gss_account structure to check
 */

int gnusocial_verify_account(gnusocial_account_t account);

/**
 * @brief Gets group information espeficied by its ID
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The group ID to get the info
 * @return A group_info structure with all the info
 */

gnusocial_group_info_t gs_get_group_info(gnusocial_account_t account, int id);

/**
 * @brief Join a group espeficied by its ID
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The group ID to join
 */

void gnusocial_join_group(gnusocial_account_t account, int id);

/**
 * @brief Leave a group especified by its ID
 * @param account A gss_account structure to authenticate the user into the server
 * @param id The group ID to leave
 */

void gnusocial_leave_group(gnusocial_account_t account, int id);

/**
 * @brief Gets little information of a number of groups especified by a 'n_groups'
 * @param account A gss_account structure to authenticate the user into the server
 * @param n_groups The number of groups to be readed
 * @param group_timeline Can be USER_GROUPS (ZERO) or SERVER_GROUPS (1)
 * @return An array of little_group_info structures with all the info
 */

gnusocial_little_group_info_t *gs_list_groups(gnusocial_account_t account,
                                              int n_groups, int group_timeline);

/**
 * @brief Start to follow a user especified by its screen_name
 * @param account A gss_account structure to authenticate the user into the server
 * @param screen_name The user screen_name to follow
 */

void gnusocial_follow_user(gnusocial_account_t account, char *screen_name);

/**
 * @brief Stop to follow a user espeficied by its screen_name
 * @param account A gss_account structure to authenticate the user into the server
 * @param screen_name The user to unfollow
 */

void gnusocial_unfollow_user(gnusocial_account_t account, char *screen_name);

/**
 * @brief Prints a list of users from a especified list
 * @param source The source list to read the users
 * @param n_users The number of users to read
 */

void gs_print_users_array_info(gnusocial_account_t account,
                               char *source, int n_users);

/**
 * @brief Sends a formatted content to the server API
 * @param account A gss_structure to authenticate into the server
 * @param send The content to send to the server
 * @param xml_doc Where to send the content using 'account'
 */

char *gnusocial_api_request(gnusocial_account_t account, char *send, char *xml_doc);

/**
 * @brief Get the number of groups a user is member of
 * @param session Session structure
 * @param username Name of the user of whom we want to know the number of groups he is
 * member of
 * @return The number of groups of which "username" is member, or a negative error code
 */

int gnusocial_get_number_of_groups(gnusocial_account_t *session, const char *username);

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
 * @param account Account structure
 * @param filename File to be created
 */
void gnusocial_export_users(gnusocial_account_t account, char *filename);

/**
 * @brief Import users from a text file
 * @param account Account structure
 * @param filename File containing exported user follows
 */
void gnusocial_import_users(gnusocial_account_t account, char *filename);

/**
 * @brief saves the avatar image for the given local user
 * @param account Account structure
 * @param username username on the local server
 * @param avatar_filename File to save
 * @returns zero on success
 */
int gs_get_user_avatar(gnusocial_account_t account, char * username,
                       char * avatar_filename);

/**
 * @brief saves the avatar image for a user being followed
 * @param account Account structure
 * @param username username being followed
 * @param avatar_filename File to save
 * @returns zero on success
 */
int gs_get_follow_avatar(gnusocial_account_t account, char * username,
                         char * avatar_filename);

#endif
