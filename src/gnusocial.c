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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int loglevel=LOG_NONE;

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
        for (i = 0; (xml_data[start_pos+i] != '<' ||xml_data[start_pos+i+1] != '/') && i < output_size; i++) {
            output[i] = xml_data[start_pos+i];
        }
        ret = (start_pos+i);
    }
    return ret;
}

struct gnusocial_status makeStatusFromRawSource(char *raw_data, int data_size)
{
    struct gnusocial_status out_status;
    char buffer[16];
    parseXml(raw_data, data_size, "<text>", 6, out_status.text, 1024);
    parseXml(raw_data, data_size, "<id>", 4, buffer, 16);
    out_status.id = atoi(buffer);
    parseXml(raw_data, data_size, "<screen_name>", 13, out_status.author_screen_name, 64);
    parseXml(raw_data, data_size, "<in_reply_to_status_id>", 23, buffer, 16);
    out_status.in_reply_to_id = atoi(buffer);
    parseXml(raw_data, data_size, "<in_reply_to_screen_name>", 25, out_status.in_reply_to_user, 64);
    parseXml(raw_data, data_size, "<created_at>", 12, out_status.date, 64);
    return out_status;
}

int FindXmlError(char *xml_data, int xml_data_size)
{
    int ret = 0;
    char error[512];
    if ((ret = parseXml(xml_data, xml_data_size, "<error>", 7, error, 512)) > 0) {
        printf("Error: %s\n", error);
    }
    return ret;
}

int get_number_of_groups(struct gnusocial_gss_account account)
{
    char source[128];
    snprintf(source, 128, "&screen_name=%s", account.user);
    char *xml_data = send_to_api(account, source, "users/show.xml");
    char error[512];
    char n_groups[32] = "0";
    int xml_data_size = strlen(xml_data);
    if (parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
    }
    else {
        parseXml(xml_data, xml_data_size, "<groups_count>", 14, n_groups, 32);
    }
    return atoi(n_groups);
}

void init_account(struct gnusocial_gss_account * acc, char * protocol,
                  char * user, char * server, char * password)
{
    sprintf(acc->protocol, "%s", protocol);
    sprintf(acc->user, "%s", user);
    sprintf(acc->server, "%s", server);
    sprintf(acc->password, "%s", password);
}
