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

int gnusocial_server_version(gnusocial_session_t *session, char *version,
			unsigned int version_size)
{
    int result = gnusocial_api_request(session, NULL, "statusnet/version.xml");
    if (!result && (session->errormsg = parser_get_error(session->xml)))
    	    result = GNUSOCIAL_API_ERROR;
    else if (!result)
    	    parseXml(session->xml, strlen(session->xml), "<version>", 9,
    	    	    version, version_size);
    return result;
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
