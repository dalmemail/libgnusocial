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
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gnusocial_export_users() is a function by Bob Mottram *
 * Copyright (C) 2017, 2018 Bob Mottram. *
 */

int gnusocial_export_users(gnusocial_session_t *session, char *filename)
{
    FILE *fp;
    char *source = "statuses/friends.xml";
    char count[32];
    snprintf(count, 32, "count=%d", 99999);
    int ret = gnusocial_api_request(session,count,source);

    if (ret)
    	    return ret;

    fp = fopen(filename, "w");
    if (!fp) {
        printf("Unable to write to file\n");
        return 1;
    }

    if (!ret && *session->xml) {
        char screen_name[64];
        char url[128];
        int start_status_point = 0;
        int real_status_point = 0;
        char *array_data = &session->xml[0];
        int i;
        int xml_data_size = strlen(session->xml);
        for (i = 0; i < 99999 && (real_status_point+13) < xml_data_size; i++) {
            parseXml(array_data, (xml_data_size-real_status_point), "<screen_name>", 13, screen_name, 64);
            parseXml(array_data, (xml_data_size-real_status_point), "<ostatus_uri>", 13, url, 128);
            start_status_point =
                parseXml(array_data, (xml_data_size-real_status_point), "</user>", 7, NULL, 0);
            fprintf(fp, "%s,%s\n", screen_name, url);
            real_status_point += start_status_point;
            array_data = &session->xml[real_status_point];
        }
    }
    else {
    	session->errormsg = calloc(1, GNUSOCIAL_ERROR_SIZE);
        snprintf(session->errormsg, GNUSOCIAL_ERROR_SIZE, "Reading users from '%s://%s/api/%s'\n",
        	session->account->protocol, session->account->server, source);
        ret = GNUSOCIAL_UNKNOWN_ERROR;
    }
    fclose(fp);
    return ret;
}

int gnusocial_follow_user(gnusocial_session_t *session, char *screen_name)
{
    char flags[64];
    snprintf(flags, sizeof(flags), "screen_name=%s", screen_name);
    return gnusocial_api_request(session, flags, GNUSOCIAL_API_RESOURCE_CREATE_FRIENDSHIP);
}

int gnusocial_get_user_info(gnusocial_session_t *session, char *source)
{
    int ret = gnusocial_api_request(session, source, GNUSOCIAL_API_RESOURCE_SHOW_USER);

    if (!ret) {
    	    session->accounts = calloc(1, sizeof(gnusocial_account_info_t));
    	    session->accounts[0] = parser_get_account_info(session->xml);
    	    session->n_accounts = 1;
    }

    return ret;
}

/* gnusocial_import_users() is a function by Bob Mottram *
 * Copyright (C) 2017, 2018 Bob Mottram. *
 */

void gnusocial_import_users(gnusocial_session_t *session, char *filename)
{
    FILE * fp;
    char send[256], line[256];
    int i;
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
            /*ret = */gnusocial_api_request(session, send, GNUSOCIAL_API_RESOURCE_CREATE_FRIENDSHIP);
            //FindXmlError(xml_data, strlen(xml_data));
        }

    fclose(fp);
}

/*int gs_print_users_array_info(gnusocial_session_t *session, char *source, int n_users)
{
    char flags[32];
    snprintf(flags, sizeof(flags), "count=%d", n_users);
    int ret = gnusocial_api_request(session, flags, source);
    if (!ret && (session->errormsg = parser_get_error(session->xml)))
    	    ret = GNUSOCIAL_API_ERROR;
    else if (*session->xml) {
        char name[64];
        char screen_name[64];
        char url[128];
        int start_status_point = 0;
        int real_status_point = 0;
        char *array_data = &session->xml[0];
        int xml_data_size = strlen(session->xml);
        int i;
        for (i = 0; i < n_users && (real_status_point+13) < xml_data_size; i++) {
            parseXml(array_data, (xml_data_size-real_status_point), "<name>", 6, name, 64);
            parseXml(array_data, (xml_data_size-real_status_point), "<screen_name>", 13, screen_name, 64);
            parseXml(array_data, (xml_data_size-real_status_point), "<ostatus_uri>", 13, url, 128);
            start_status_point = parseXml(array_data, (xml_data_size-real_status_point), "</user>", 7, NULL, 0);
            printf("%s,%s,%s\n", name, screen_name, url);
            real_status_point += start_status_point;
            array_data = &session->xml[real_status_point];
        }
    }
    else {
        printf("Error: Reading %d users from '%s://%s/api/%s'\n", n_users, account.protocol, account.server, source);
    }
    return ret;
}*/

int gnusocial_unfollow_user(gnusocial_session_t *session, char *screen_name)
{
    char flags[64];
    snprintf(flags, sizeof(flags), "screen_name=%s", screen_name);
    return gnusocial_api_request(session, flags, GNUSOCIAL_API_RESOURCE_DESTROY_FRIENDSHIP);
}