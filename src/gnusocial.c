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

int gs_get_number_of_groups(gnusocial_account_t account)
{
    char source[128];
    snprintf(source, 128, "&screen_name=%s", account.user);
    char *xml_data = gnusocial_api_request(account, source, "users/show.xml");
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

void gnusocial_init_account(gnusocial_account_t * acc, char * protocol,
                     char * user, char * server, char * password,
                     char * socks_proxy)
{
    snprintf(acc->protocol, 8, "%s", protocol);
    snprintf(acc->user, 64, "%s", user);
    snprintf(acc->server, 32, "%s", server);
    snprintf(acc->password, 64, "%s", password);
    if (socks_proxy != NULL)
        snprintf(acc->socks_proxy, 64, "%s", socks_proxy);
    else
        acc->socks_proxy[0] = 0;
}
