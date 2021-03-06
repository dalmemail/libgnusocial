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
#include <stddef.h>

#include <ctype.h>
#include "constants.h"

struct Chunk {
    char *memory;
    size_t size;
};

/* This in-memory cURL callback is from
   https://curl.haxx.se/libcurl/c/getinmemory.html */
static size_t cb_writeXmlChunk(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct Chunk *mem = (struct Chunk *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int gnusocial_api_request(gnusocial_session_t *session, char *send, char *xml_doc)
{
    int result = GNUSOCIAL_REQUEST_SUCCEED;

    if (!session)
    	    return GNUSOCIAL_ERROR_NULL_SESSION;
    
    if (!session->account)
    	    return GNUSOCIAL_ERROR_NULL_ACCOUNT;

    gnusocial_account_t *account = session->account;
    // Just remove the values from the last request
    gnusocial_session_reset(session);

    CURLcode err;
    char url[MAX_URL];
    char userpwd[GNUSOCIAL_ACCOUNT_USERNAME_SIZE+GNUSOCIAL_ACCOUNT_PASSWORD_SIZE-1];
    snprintf(userpwd, sizeof(userpwd), "%s:%s", account->user, account->password);
    snprintf(url, sizeof(url), "%s://%s/api/%s", account->protocol, account->server, xml_doc);
    struct Chunk xml;
    xml.memory = (char *)malloc(1);
    xml.size = 0;
    CURL *curl = curl_easy_init();
    // libcurl never reads .curlrc:
    curl_easy_setopt(curl, CURLOPT_CAPATH, "/etc/ssl/certs/" );
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd);
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb_writeXmlChunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&xml);

    if (account->socks_proxy[0] != 0) {
        curl_easy_setopt(curl, CURLOPT_PROXY, &account->socks_proxy[0]);
        curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
    }

    if (send) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, send);
    }

    err = curl_easy_perform(curl);
    if (err == CURLE_OK)
        session->xml = xml.memory;
    else if ((session->errormsg = calloc(1, CURL_ERROR_SIZE))) {
        snprintf(session->errormsg, CURL_ERROR_SIZE, "%s", curl_easy_strerror(err));
        result = GNUSOCIAL_CURL_ERROR;
    }
    else
        result = GNUSOCIAL_ERROR_NULL_MEMORY_ALLOCATED;

    curl_easy_cleanup(curl);

    if (!result && *session->xml && (session->errormsg = parser_get_error(session->xml)))
    	    result = GNUSOCIAL_API_ERROR;

    return result;
}

int gnusocial_verify_account(gnusocial_session_t *session)
{
    int ret = gnusocial_api_request(session, NULL, GNUSOCIAL_API_RESOURCE_VERIFY_CREDENTIALS);
    if ((session->errormsg = parser_get_error(session->xml)))
    	    ret = GNUSOCIAL_API_ERROR;
    // Temporal until we found if this is needed
    else if (parseXml(session->xml, strlen(session->xml), "<screen_name>", 13, NULL, 0) < 0) {
    	session->errormsg = calloc(1, GNUSOCIAL_ERROR_SIZE);
        snprintf(session->errormsg, GNUSOCIAL_ERROR_SIZE, "Error: Connecting to @%s@%s\n",
        	session->account->user, session->account->server);
        ret = GNUSOCIAL_ERROR_INVALID_CREDENTIALS;
    }
    return ret;
}