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

#include <curl/curl.h>
#include "gnusocial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gs_export_users() is a function by Bob Mottram *
 * Copyright (C) 2017, 2018 Bob Mottram. *
 */

void gs_export_users(gnusocial_account_t account, char *filename)
{
    FILE * fp;
    char * source = "statuses/friends.xml";
    char count[32];
    snprintf(count, 32, "count=%d", 99999);
    char *xml_data = gs_send_to_api(account,count,source);
    int xml_data_size = strlen(xml_data);
    char error[512];

    fp = fopen(filename, "w");
    if (!fp) {
        printf("Unable to write to file\n");
        return;
    }

    if (gs_parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
    }
    else if (xml_data_size > 0) {
        char screen_name[64];
        char url[128];
        int start_status_point = 0;
        int real_status_point = 0;
        char *array_data;
        array_data = &xml_data[0];
        int i;
        for (i = 0; i < 99999 && (real_status_point+13) < xml_data_size; i++) {
            gs_parseXml(array_data, (xml_data_size-real_status_point), "<screen_name>", 13, screen_name, 64);
            gs_parseXml(array_data, (xml_data_size-real_status_point), "<ostatus_uri>", 13, url, 128);
            start_status_point =
                gs_parseXml(array_data, (xml_data_size-real_status_point), "</user>", 7, "", 0);
            fprintf(fp, "%s,%s\n", screen_name, url);
            real_status_point += start_status_point;
            array_data = &xml_data[real_status_point];
        }
    }
    else {
        printf("Error: Reading users from '%s://%s/api/%s'\n", account.protocol, account.server, source);
    }
    free(xml_data);
    fclose(fp);
}

void gs_follow_user(gnusocial_account_t account, char *screen_name)
{
    char send[79];
    snprintf(send, 79, "screen_name=%s", screen_name);
    char *xml_data = gs_send_to_api(account, send, "friendships/create.xml");
    gs_FindXmlError(xml_data, strlen(xml_data));
    free(xml_data);
}

gnusocial_account_info_t gs_get_user_info(gnusocial_account_t account, char *source)
{
    char *xml_data = gs_send_to_api(account, source, "users/show.xml");
    char error[512];
    char output[512];
    int xml_data_size = strlen(xml_data);
    gnusocial_account_info_t info;
    if (gs_parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
        info.screen_name[0] = '\0';
    }
    else {
        printf("%s\n", xml_data);

        if (gs_parseXml(xml_data, xml_data_size, "<name>", 6, output, MAX_ACCOUNT_NAME) > 0) {
            strncpy(info.name, output, MAX_ACCOUNT_NAME);
        }
        else {
            info.name[0] = '?';
            info.name[1] = '\0';
        }
        if (gs_parseXml(xml_data, xml_data_size, "<screen_name>", 13, output, MAX_SCREEN_NAME) > 0) {
            strncpy(info.screen_name, output, MAX_SCREEN_NAME);
        }
        else {
            info.screen_name[0] = '?';
            info.screen_name[1] = '\0';
        }
        if (gs_parseXml(xml_data, xml_data_size, "<location>", 10, output, MAX_LOCATION) > 0) {
            strncpy(info.location, output, MAX_LOCATION);
        }
        else {
            info.location[0] = '?';
            info.location[1] = '\0';
        }
        if (gs_parseXml(xml_data, xml_data_size, "<description>", 13, output, MAX_DESCRIPTION) > 0) {
            strncpy(info.description, output, MAX_DESCRIPTION);
        }
        else {
            info.description[0] = '?';
            info.description[1] = '\0';
        }
        if (gs_parseXml(xml_data, xml_data_size, "<url>", 5, output, MAX_URL) > 0) {
            strncpy(info.url, output, MAX_URL);
        }
        else {
            info.url[0] = '?';
            info.url[1] = '\0';
        }
        if (gs_parseXml(xml_data, xml_data_size, "<followers_count>", 17, output, 512) > 0) {
            info.followers = atoi(output);
        }
        else {
            info.followers = -1;
        }
        if (gs_parseXml(xml_data, xml_data_size, "<friends_count>", 15, output, 512) > 0) {
            info.friends = atoi(output);
        }
        else {
            info.friends = -1;
        }
        if (gs_parseXml(xml_data, xml_data_size, "<statuses_count>", 16, output, 512) > 0) {
            info.statuses = atoi(output);
        }
        else {
            info.statuses = -1;
        }
    }
    free(xml_data);
    return info;
}

/* gs_export_users() is a function by Bob Mottram *
 * Copyright (C) 2017, 2018 Bob Mottram. *
 */

void gs_import_users(gnusocial_account_t account, char *filename)
{
    FILE * fp;
    char send[256], line[256];
    int i;
    char *xml_data;
    const char field_separator = ',';

    fp = fopen(filename,"r");
    if (fp==0) {
        printf("File not found\n");
        return;
    }

    while (!feof(fp))
        if (fgets(line , 255 , fp) != NULL ) {
            /* get the last field */
            i = strlen(line)-1;
            while ((i > 1) && (line[i] != field_separator))
                i--;
            if (line[i] != field_separator)
                continue;
            i++;

            snprintf(send, 255, "ostatus_uri=%s", (char*)&line[i]);
            printf("%s\n", send);
            xml_data = gs_send_to_api(account, send, "friendships/create.xml");
            gs_FindXmlError(xml_data, strlen(xml_data));
            free(xml_data);
        }

    fclose(fp);
    printf("Done\n", send);
}

void gs_print_users_array_info(gnusocial_account_t account, char *source, int n_users)
{
    char count[32];
    snprintf(count, 32, "count=%d", n_users);
    char *xml_data = gs_send_to_api(account,count,source);
    int xml_data_size = strlen(xml_data);
    char error[512];
    if (gs_parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
    }
    else if (xml_data_size > 0) {
        char name[64];
        char screen_name[64];
        char url[128];
        int start_status_point = 0;
        int real_status_point = 0;
        char *array_data;
        array_data = &xml_data[0];
        int i;
        for (i = 0; i < n_users && (real_status_point+13) < xml_data_size; i++) {
            gs_parseXml(array_data, (xml_data_size-real_status_point), "<name>", 6, name, 64);
            gs_parseXml(array_data, (xml_data_size-real_status_point), "<screen_name>", 13, screen_name, 64);
            gs_parseXml(array_data, (xml_data_size-real_status_point), "<ostatus_uri>", 13, url, 128);
            start_status_point = gs_parseXml(array_data, (xml_data_size-real_status_point), "</user>", 7, "", 0);
            printf("%s,%s,%s\n", name, screen_name, url);
            real_status_point += start_status_point;
            array_data = &xml_data[real_status_point];
        }
    }
    else {
        printf("Error: Reading %d users from '%s://%s/api/%s'\n", n_users, account.protocol, account.server, source);
    }
    free(xml_data);
}

void unfollow_user(gnusocial_account_t account, char *screen_name)
{
    char send[256];
    snprintf(send, 256, "screen_name=%s", screen_name);
    char *xml_data = gs_send_to_api(account,send,"friendships/destroy.xml");
    char error[512];
    int xml_data_size = strlen(xml_data);
    if (gs_parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
    }
    free(xml_data);
}

gnusocial_account_info_t gs_datatoaccount(char *xml_data, int xml_data_size)
{
    gnusocial_account_info_t info;
    char output[512];

    if (gs_parseXml(xml_data, xml_data_size, "<name>", 6, output, 512) > 0) {
        strncpy(info.name, output, MAX_ACCOUNT_NAME);
    }
    else {
        info.name[0] = '?';
        info.name[1] = '\0';
    }
    if (gs_parseXml(xml_data, xml_data_size, "<screen_name>", 13, output, MAX_SCREEN_NAME) > 0) {
        strncpy(info.screen_name, output, MAX_SCREEN_NAME);
    }
    else {
        info.name[0] = '?';
        info.name[1] = '\0';
    }
    if (gs_parseXml(xml_data, xml_data_size, "<location>", 10, output, MAX_LOCATION) > 0) {
        strncpy(info.location, output, MAX_LOCATION);
    }
    else {
        info.name[0] = '?';
        info.name[1] = '\0';
    }
    if (gs_parseXml(xml_data, xml_data_size, "<description>", 13, output, MAX_DESCRIPTION) > 0) {
        strncpy(info.description, output, MAX_DESCRIPTION);
    }
    else {
        info.name[0] = '?';
        info.name[1] = '\0';
    }
    if (gs_parseXml(xml_data, xml_data_size, "<url>", 5, output, MAX_URL) > 0) {
        strncpy(info.url, output, MAX_URL);
    }
    else {
        info.name[0] = '?';
        info.name[1] = '\0';
    }
    if (gs_parseXml(xml_data, xml_data_size, "<followers_count>", 17, output, 512) > 0) {
        info.followers = atoi(output);
    }
    else {
        info.followers = -1;
    }
    if (gs_parseXml(xml_data, xml_data_size, "<friends_count>", 15, output, 512) > 0) {
        info.friends = atoi(output);
    }
    else {
        info.friends = -1;
    }
    if (gs_parseXml(xml_data, xml_data_size, "<statuses_count>", 16, output, 512) > 0) {
        info.statuses = atoi(output);
    }
    else {
        info.statuses = -1;
    }
    if (gs_parseXml(xml_data, xml_data_size, "<profile_image_url>", 19, output, MAX_URL) > 0) {
        strncpy(info.profile_image_url, output, MAX_URL);
    }
    else {
        info.profile_image_url[0] = 0;
    }
    if (gs_parseXml(xml_data, xml_data_size, "<profile_image_url_profile_size>", 32, output, MAX_URL) > 0) {
        strncpy(info.profile_image_url_profile_size, output, MAX_URL);
    }
    else {
        info.profile_image_url_profile_size[0] = 0;
    }
    return info;
}

gnusocial_account_info_t gs_get_my_account_info(gnusocial_account_t account, int *result)
{
    char send[79];
    snprintf(send, 79, "screen_name=%s", account.user);
    char *xml_data = gs_send_to_api(account, send, "users/show.xml");
    int xml_data_size = strlen(xml_data);
    gnusocial_account_info_t info;
    if (gs_FindXmlError(xml_data, xml_data_size) < 0) {
        printf("%s\n", xml_data);
        info = gs_datatoaccount(xml_data, xml_data_size);
        *result = 0;
    }
    free(xml_data);
    return info;
}