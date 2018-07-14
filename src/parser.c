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
        for (i = 0; (xml_data[start_pos+i] != '<' ||xml_data[start_pos+i+1] != '/') && i < output_size; i++) {
            output[i] = xml_data[start_pos+i];
        }
        ret = (start_pos+i);
    }
    return ret;
}

gnusocial_status_t makeStatusFromRawSource(char *raw_data, int data_size)
{
    gnusocial_status_t out_status;
    char buffer[16];
    parseXml(raw_data, data_size, "<text>", 6, out_status.text, sizeof(out_status.text));
    parseXml(raw_data, data_size, "<id>", 4, buffer, sizeof(buffer));
    out_status.id = atoi(buffer);
    parseXml(raw_data, data_size, "<screen_name>", 13, out_status.author_screen_name, sizeof(out_status.author_screen_name));
    parseXml(raw_data, data_size, "<in_reply_to_status_id>", 23, buffer, sizeof(buffer));
    out_status.in_reply_to_id = atoi(buffer);
    parseXml(raw_data, data_size, "<in_reply_to_screen_name>", 25, out_status.in_reply_to_user, sizeof(out_status.in_reply_to_user));
    parseXml(raw_data, data_size, "<created_at>", 12, out_status.date, sizeof(out_status.date));
    return out_status;
}

int FindXmlError(char *xml_data, int xml_data_size)
{
    int ret = 0;
    char error[512];
    if ((ret = parseXml(xml_data, xml_data_size, "<error>",
                           7, error, sizeof(error))) > 0) {
        printf("Error: %s\n", error);
    }
    return ret;
}