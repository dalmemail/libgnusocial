/*
 * Copyright (C) 2018 Daniel Martin
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
 
#include "gnusocial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parseXml(char *xml_data, int xml_data_size, char *tofind, int tofind_size, char *output, int output_size)
{
    int pos = 0;
    int act_pos = 0;
    int start_pos = 0;
    int ret = -1;
    int i;
    memset(output, '\0', output_size);
    for (i = pos; act_pos != tofind_size && i < xml_data_size; i++) {
        if (tofind[act_pos] == xml_data[i]) {
            act_pos++;
            if (act_pos == tofind_size) {
                pos = i;
                ret = 0;
            }
        }
        else {
            act_pos = 0;
        }
    }
    start_pos = pos+1;
    if (ret == 0) {
        for (i = 0; (xml_data[start_pos+i] != '<' ||xml_data[start_pos+i+1] != '/') && i < (output_size-1); i++) {
            output[i] = xml_data[start_pos+i];
        }
        ret = (start_pos+i);
        output[i] = 0;
    }
    return ret;
}

gnusocial_status_t parser_get_status(char *xml_data)
{
    gnusocial_status_t status;
    int data_size = strlen(xml_data);
    char buffer[16];
    if (parseXml(xml_data, data_size, "<text>", 6, status.text, sizeof(status.text)) < 0)
    	    status.text[0] = 0;

    if (parseXml(xml_data, data_size, "<id>", 4, buffer, sizeof(buffer)) > 0)
    	    status.id = atoi(buffer);
    else
    	    status.id = 0;

    if (parseXml(xml_data, data_size, "<screen_name>", 13, status.author_screen_name, sizeof(status.author_screen_name)) < 0)
    	    status.author_screen_name[0] = 0;

    if (parseXml(xml_data, data_size, "<in_reply_to_status_id>", 23, buffer, sizeof(buffer)) > 0)
    	    status.in_reply_to_id = atoi(buffer);
    else
    	    status.in_reply_to_id = 0;

    if (parseXml(xml_data, data_size, "<in_reply_to_screen_name>", 25, status.in_reply_to_user, sizeof(status.in_reply_to_user)) < 0)
    	    status.in_reply_to_user[0] = 0;

    if (parseXml(xml_data, data_size, "<created_at>", 12, status.date, sizeof(status.date)) < 0)
    	    status.date[0] = 0;

    return status;
}

gnusocial_group_info_t parser_get_group_info(char *xml_data)
{
    gnusocial_group_info_t group;
    int xml_data_size = strlen(xml_data);
    char buffer[256];

    if (parseXml(xml_data, xml_data_size, "<id>", 4, buffer, sizeof(buffer)) < 0)
        group.id = 0;
    else
    	group.id = atoi(buffer);

    if (parseXml(xml_data, xml_data_size, "<url>", 5, group.url, sizeof(group.url)) < 0)
        group.url[0] = 0;

    if (parseXml(xml_data, xml_data_size, "<nickname>", 10, group.nickname, sizeof(group.nickname)) < 0)
        group.nickname[0] = 0;

    if (parseXml(xml_data, xml_data_size, "<fullname>", 10, group.fullname, sizeof(group.fullname)) < 0)
        group.fullname[0] = 0;

    if (parseXml(xml_data, xml_data_size, "<member>", 8, buffer, sizeof(buffer)) > 0) {
        if (!strcmp(buffer, "true")) {
            group.member = 1;
        }
        else {
            group.member = 0;
        }
    }
    else {
        group.member = GNUSOCIAL_API_ERROR_TAG_NOT_FOUND;
    }

    if (parseXml(xml_data, xml_data_size, "<admin_count>", 13, buffer, sizeof(buffer)) > 0)
        group.admins = atoi(buffer);
    else
            group.admins = GNUSOCIAL_API_ERROR_TAG_NOT_FOUND;

    if (parseXml(xml_data, xml_data_size, "<member_count>", 14, buffer, sizeof(buffer)) > 0)
        group.members = atoi(buffer);
    else
        group.members = GNUSOCIAL_API_ERROR_TAG_NOT_FOUND;

    if (parseXml(xml_data, xml_data_size, "<description>", 13, group.description, sizeof(group.description)) < 0)
        group.description[0] = 0;

    return group;
}

gnusocial_account_info_t parser_get_account_info(char *xml_data)
{
    gnusocial_account_info_t info;
    int xml_data_size = strlen(xml_data);
    char buffer[256];

    if (parseXml(xml_data, xml_data_size, "<name>", 6, info.name, sizeof(info.name)) < 0)
        info.name[0] = 0;

    if (parseXml(xml_data, xml_data_size, "<screen_name>", 13, info.screen_name, sizeof(info.screen_name)) < 0)
        info.screen_name[0] = 0;

    if (parseXml(xml_data, xml_data_size, "<location>", 10, info.location, sizeof(info.location)) < 0)
        info.location[0] = 0;

    if (parseXml(xml_data, xml_data_size, "<description>", 13, info.description, sizeof(info.description)) < 0)
        info.description[0] = 0;

    if (parseXml(xml_data, xml_data_size, "<url>", 5, info.url, sizeof(info.url)) < 0)
        info.url[0] = 0;

    if (parseXml(xml_data, xml_data_size, "<followers_count>", 17, buffer, sizeof(buffer)) > 0)
        info.followers = atoi(buffer);
    else
        info.followers = GNUSOCIAL_API_ERROR_TAG_NOT_FOUND;

    if (parseXml(xml_data, xml_data_size, "<friends_count>", 15, buffer, sizeof(buffer)) > 0)
        info.friends = atoi(buffer);
    else
        info.friends = GNUSOCIAL_API_ERROR_TAG_NOT_FOUND;

    if (parseXml(xml_data, xml_data_size, "<statuses_count>", 16, buffer, sizeof(buffer)) > 0)
        info.statuses = atoi(buffer);
    else
        info.statuses = GNUSOCIAL_API_ERROR_TAG_NOT_FOUND;

    if (parseXml(xml_data, xml_data_size, "<profile_image_url>", 19, info.profile_image_url, sizeof(info.profile_image_url)) < 0)
        info.profile_image_url[0] = 0;

    if (parseXml(xml_data, xml_data_size, "<profile_image_url_profile_size>", 32, info.profile_image_url_profile_size, sizeof(info.profile_image_url_profile_size)) < 0)
        info.profile_image_url_profile_size[0] = 0;

    return info;
}

char *parser_get_error(char *xml_data)
{
    char *errormsg = calloc(1, GNUSOCIAL_ERROR_SIZE);
    if (parseXml(xml_data, strlen(xml_data), "<error>", 7, errormsg, GNUSOCIAL_ERROR_SIZE) <= 0) {
        free(errormsg);
        return NULL;
    }
    return errormsg;
}