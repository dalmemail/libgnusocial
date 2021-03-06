/*
 * Copyright (C) 2016, 2018 Daniel Martín
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

#define GNUSOCIAL_FAVORITES_TIMELINE "favorites.xml"
#define GNUSOCIAL_PUBLIC_TIMELINE "statuses/public_timeline.xml"
#define GNUSOCIAL_HOME_TIMELINE "statuses/home_timeline.xml"
#define GNUSOCIAL_MENTIONS_TIMELINE "statuses/mentions.xml"

#define GNUSOCIAL_FOLLOWERS_LIST "statuses/followers.xml"
#define GNUSOCIAL_FRIENDS_LIST "statuses/friends.xml"

#define GNUSOCIAL_API_RESOURCE_VERIFY_CREDENTIALS "account/verify_credentials.xml"
#define GNUSOCIAL_API_RESOURCE_SERVER_VERSION "statusnet/version.xml"
#define GNUSOCIAL_API_RESOURCE_JOIN_GROUP "statusnet/groups/join.xml"
#define GNUSOCIAL_API_RESOURCE_LEAVE_GROUP "statusnet/groups/leave.xml"
#define GNUSOCIAL_API_RESOURCE_SHOW_GROUP "statusnet/groups/show.xml"
#define GNUSOCIAL_API_RESOURCE_SHOW_USER "users/show.xml"
#define GNUSOCIAL_API_RESOURCE_UPDATE_STATUS "statuses/update.xml"
#define GNUSOCIAL_API_RESOURCE_DESTROY_STATUS "statuses/destroy.xml"
#define GNUSOCIAL_API_RESOURCE_CREATE_FAVORITE "favorites/create.xml"
#define GNUSOCIAL_API_RESOURCE_DESTROY_FAVORITE "favorites/destroy.xml"
#define GNUSOCIAL_API_RESOURCE_SHOW_STATUS "statuses/show.xml"
#define GNUSOCIAL_API_RESOURCE_CREATE_FRIENDSHIP "friendships/create.xml"
#define GNUSOCIAL_API_RESOURCE_DESTROY_FRIENDSHIP "friendships/destroy.xml"

#define MAX_ACCOUNT_NAME 64
#define MAX_SCREEN_NAME 192
#define MAX_LOCATION 192
#define MAX_DESCRIPTION 256

#define MAX_URL 256

#define MAX_GROUP_NICKNAME 64
#define MAX_GROUP_FULLNAME 256

#define MAX_SOURCE 32

#define GNUSOCIAL_ACCOUNT_PROTOCOL_SIZE 8
#define GNUSOCIAL_ACCOUNT_USERNAME_SIZE 64
#define GNUSOCIAL_ACCOUNT_PASSWORD_SIZE 64
#define GNUSOCIAL_ACCOUNT_SERVER_SIZE 32
#define GNUSOCIAL_ACCOUNT_PROXY_SIZE 64

#define GNUSOCIAL_ERROR_SIZE 256

// Error codes
#define GNUSOCIAL_REQUEST_SUCCEED 0
#define GNUSOCIAL_ERROR_INVALID_PROTOCOL_SIZE -1
#define GNUSOCIAL_ERROR_INVALID_USERNAME_SIZE -2
#define GNUSOCIAL_ERROR_INVALID_SERVER_SIZE -3
#define GNUSOCIAL_ERROR_INVALID_PASSWORD_SIZE -4
#define GNUSOCIAL_ERROR_INVALID_PROXY_SIZE -5
#define GNUSOCIAL_ERROR_NULL_MEMORY_ALLOCATED -6
#define GNUSOCIAL_ERROR_NULL_SESSION -7
#define GNUSOCIAL_ERROR_NULL_ACCOUNT -8
#define GNUSOCIAL_CURL_ERROR -9
#define GNUSOCIAL_API_ERROR -10
#define GNUSOCIAL_API_ERROR_TAG_NOT_FOUND -11
#define GNUSOCIAL_ERROR_INVALID_CREDENTIALS -12
#define GNUSOCIAL_UNKNOWN_ERROR -13
