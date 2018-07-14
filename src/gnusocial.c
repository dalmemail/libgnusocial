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
#include <string.h>

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

static int check_account_credentials_length(char *protocol, char *user, char *server,
			char *password, char *socks_proxy)
{
	if (strlen(protocol) >= GNUSOCIAL_ACCOUNT_PROTOCOL_SIZE)
		return GNUSOCIAL_ERROR_INVALID_PROTOCOL_SIZE;
	
	if (strlen(user) >= GNUSOCIAL_ACCOUNT_USERNAME_SIZE)
		return GNUSOCIAL_ERROR_INVALID_USERNAME_SIZE;
	
	if (strlen(server) >= GNUSOCIAL_ACCOUNT_SERVER_SIZE)
		return GNUSOCIAL_ERROR_INVALID_SERVER_SIZE;
	
	if (strlen(password) >= GNUSOCIAL_ACCOUNT_PASSWORD_SIZE)
		return GNUSOCIAL_ERROR_INVALID_PASSWORD_SIZE;
	
	if (strlen(socks_proxy) >= GNUSOCIAL_ACCOUNT_PROXY_SIZE)
		return GNUSOCIAL_ERROR_INVALID_PROXY_SIZE;
	
	return 0;
}

int gnusocial_set_account(gnusocial_session_t *session, char *protocol,
			char *user, char *server, char *password, char *socks_proxy)
{
	int rc = check_account_credentials_length(protocol, user, server, password, socks_proxy);
	if (rc < 0)
		return rc;

	session->account = calloc(1, sizeof(gnusocial_account_t));
	if (!session->account)
		return GNUSOCIAL_ERROR_NULL_MEMORY_ALLOCATED;

	snprintf(session->account->protocol, GNUSOCIAL_ACCOUNT_PROTOCOL_SIZE, "%s", protocol);
	snprintf(session->account->user, GNUSOCIAL_ACCOUNT_USERNAME_SIZE, "%s", user);
	snprintf(session->account->server, GNUSOCIAL_ACCOUNT_SERVER_SIZE, "%s", server);
	snprintf(session->account->password, GNUSOCIAL_ACCOUNT_PASSWORD_SIZE, "%s", password);
	if (socks_proxy)
		snprintf(session->account->socks_proxy, GNUSOCIAL_ACCOUNT_PROXY_SIZE, "%s", socks_proxy);
	else
		session->account->socks_proxy[0] = 0;

	return 0;
}
