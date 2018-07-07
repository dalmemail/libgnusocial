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

#include "gnusocial.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

#define USER_GROUPS 0
#define SERVER_GROUPS 1

char timelines[2][64] = {"statusnet/groups/list.xml", "statusnet/groups/list_all.xml"};

void gs_join_group(gnusocial_account_t account, int id)
{
    char send[16];
    snprintf(send, 16, "id=%d", id);
    char *xml_data = gs_send_to_api(account, send, "statusnet/groups/join.xml");
    FindXmlError(xml_data, strlen(xml_data));
    free(xml_data);
}

void gs_leave_group(gnusocial_account_t account, int id)
{
    char send[16];
    snprintf(send, 16, "id=%d", id);
    char *xml_data = gs_send_to_api(account, send, "statusnet/groups/leave.xml");
    char error[512];
    int xml_data_size = strlen(xml_data);
    if (parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
    }
    free(xml_data);
}

gnusocial_group_info_t gs_get_group_info(gnusocial_account_t account, int id)
{
    char send[16];
    snprintf(send, 16, "id=%d", id);
    char *xml_data = gs_send_to_api(account, send, "statusnet/groups/show.xml");
    char error[512];
    char output[512];
    int xml_data_size = strlen(xml_data);
    gnusocial_group_info_t group;
    if (parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
        group.id = -1;
    }
    else {
        group.id = id;
        if (parseXml(xml_data, xml_data_size, "<url>", 5, output, MAX_URL) > 0) {
            strncpy(group.url, output, MAX_URL);
        }
        else {
            group.url[0] = '?';
            group.url[1] = '\0';
        }
        if (parseXml(xml_data, xml_data_size, "<nickname>", 10, output, MAX_GROUP_NICKNAME) > 0) {
            strncpy(group.nickname, output, MAX_GROUP_NICKNAME);
        }
        else {
            group.nickname[0] = '?';
            group.nickname[1] = '\0';
        }
        if (parseXml(xml_data, xml_data_size, "<fullname>", 10, output, MAX_GROUP_FULLNAME) > 0) {
            strncpy(group.fullname, output, MAX_GROUP_FULLNAME);
        }
        else {
            group.fullname[0] = '?';
            group.fullname[1] = '\0';
        }
        if (parseXml(xml_data, xml_data_size, "<member>", 8, output, 512) > 0) {
            if (strcmp(output, "true") == 0) {
                group.member = 1;
            }
            else {
                group.member = 0;
            }
        }
        else {
            group.member = -1;
        }
        if (parseXml(xml_data, xml_data_size, "<admin_count>", 13, output, 512) > 0) {
            group.admins = atoi(output);
        }
        else {
            group.admins = -1;
        }
        if (parseXml(xml_data, xml_data_size, "<member_count>", 14, output, 512) > 0) {
            group.members = atoi(output);
        }
        else {
            group.members = -1;
        }
        if (parseXml(xml_data, xml_data_size, "<description>", 13, output, MAX_DESCRIPTION) > 0) {
            strncpy(group.description, output, MAX_DESCRIPTION);
        }
        else {
            group.description[0] = '?';
            group.description[1] = '\0';
        }
    }
    free(xml_data);
    return group;
}

gnusocial_little_group_info_t *gs_list_groups(gnusocial_account_t account,
                                              int n_groups, int group_timeline)
{
    char count[32];
    snprintf(count, 32, "count=%d", n_groups);
    char *xml_data =
        gs_send_to_api(account,count,timelines[group_timeline]);
    char error[512];
    int xml_data_size = strlen(xml_data);
    gnusocial_little_group_info_t *groups =
        (gnusocial_little_group_info_t*)malloc(n_groups * sizeof(gnusocial_group_info_t));
    int i;
    for (i = 0; i < n_groups; i++) {
        groups[i].id = 0;
    }
    if (parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
    }
    else if (xml_data_size > 0) {
        int start_status_point = 0;
        int real_status_point = 0;
        char *array_data;
        char id[16];
        array_data = &xml_data[0];
        for (i = 0; i < n_groups && (real_status_point+13) < xml_data_size; i++) {
            parseXml(array_data, (xml_data_size-real_status_point), "<id>", 4, id, 16);
            groups[i].id = atoi(id);
            parseXml(array_data, (xml_data_size-real_status_point), "<nickname>", 10, groups[i].nickname, 64);
            parseXml(array_data, (xml_data_size-real_status_point), "<description>", 13, groups[i].description, 256);
            start_status_point = parseXml(array_data, (xml_data_size-real_status_point), "</group>", 8, "", 0);
            real_status_point += start_status_point;
            array_data = &xml_data[real_status_point];
        }
    }
    else {
        printf("Error: Reading '%d' groups from '%s:%s/api/%s'\n", n_groups, account.protocol, account.server, timelines[group_timeline]);
    }
    free(xml_data);
    return groups;
}