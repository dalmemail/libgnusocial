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

int gnusocial_read_timeline(gnusocial_session_t *session,
                                     char *timeline, int n_status)
{
    session->status = calloc(n_status, sizeof(gnusocial_status_t));
    char flags[32];
    snprintf(flags, sizeof(flags), "count=%d", n_status);
    int ret = gnusocial_api_request(session, flags, timeline);

    if (!ret && *session->xml) {
        int i;
        int start_status_point = 0;
        int real_status_point = 0;
        char *status_data;
        status_data = &session->xml[0];
        int xml_data_size = strlen(session->xml);
        for (i = 0; i < n_status && (real_status_point+13) < xml_data_size; i++) {
            session->status[i] = parser_get_status(status_data);
            start_status_point = parseXml(status_data, (xml_data_size-real_status_point), "</status>", 9, NULL, 0);
            if (start_status_point < 0) {
            	    session->n_status = i;
            	    return ret;
            }
            real_status_point += start_status_point;
            status_data = &session->xml[real_status_point];
        }
    }
    return ret;
}
