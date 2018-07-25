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

int gnusocial_join_group(gnusocial_session_t *session, int id)
{
    char flags[16];
    snprintf(flags, sizeof(flags), "id=%d", id);
    return gnusocial_api_request(session, flags, "statusnet/groups/join.xml");
}

int gnusocial_leave_group(gnusocial_session_t *session, int id)
{
    char flags[16];
    snprintf(flags, sizeof(flags), "id=%d", id);
    return gnusocial_api_request(session, flags, "statusnet/groups/leave.xml");
}

int gnusocial_get_group_info(gnusocial_session_t *session, int id)
{
    char flags[16];
    snprintf(flags, sizeof(flags), "id=%d", id);
    int ret = gnusocial_api_request(session, flags, "statusnet/groups/show.xml");
    if (!ret) {
       	session->groups = calloc(1, sizeof(gnusocial_group_info_t));
       	session->groups[0] = parser_get_group_info(session->xml);
        session->n_groups = 1;
    }
    return ret;
}

int gnusocial_list_groups(gnusocial_session_t *session, int n_groups, char *timeline)
{
    char flags[16];
    snprintf(flags, sizeof(flags), "count=%d", n_groups);
    int ret = gnusocial_api_request(session, flags, timeline);
    if (ret)
        return ret;
    session->groups = calloc(n_groups, sizeof(gnusocial_group_info_t));
    session->n_groups = n_groups;
    if (*session->xml) {
    	int xml_data_size = strlen(session->xml);
    	char *xml_data = session->xml;
        int start_status_point = 0;
        int real_status_point = 0;
        char *array_data = &xml_data[0];
        int i;
        for (i = 0; i < n_groups && (real_status_point+13) < xml_data_size; i++) {
            session->groups[i] = parser_get_group_info(array_data);
            start_status_point = parseXml(array_data, (xml_data_size-real_status_point), "</group>", 8, NULL, 0);

            if (start_status_point < 0) {
            	    session->n_groups = i+1;
            	    return ret;
            }

            real_status_point += start_status_point;
            array_data = &xml_data[real_status_point];
        }
    }
    else {
        snprintf(session->errormsg, GNUSOCIAL_ERROR_SIZE, "Reading '%d' groups from '%s:%s/api/%s'\n",
        	n_groups, session->account->protocol, session->account->server, timeline);
        ret = GNUSOCIAL_UNKNOWN_ERROR;
    }
    return ret;
}

int gnusocial_get_number_of_groups(gnusocial_session_t *session, const char *username)
{
    char flags[128];
    snprintf(flags, sizeof(flags), "&screen_name=%s", username);
    int request_code = gnusocial_api_request(session, flags, "users/show.xml");
    if (request_code < 0)
    	    return request_code;

    char n_groups[32];
    if (parseXml(session->xml, strlen(session->xml), "<groups_count>", 14, n_groups, 32) > 0)
        return atoi(n_groups);
    else
        return GNUSOCIAL_API_ERROR_TAG_NOT_FOUND;
}