/*
 * Copyright (C) 2016, 2017, 2018 Daniel Martín
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

#include <curl/curl.h>

int gnusocial_reply_status(gnusocial_session_t *session, int id, char *msg)
{
	int ret = GNUSOCIAL_CURL_ERROR;
	/* cURL functionality used just to URIencode the msg */
	CURL *curl = curl_easy_init();
	if(curl) {
		char *encoded_msg = curl_easy_escape(curl, msg, strlen(msg));
		if(encoded_msg) {
			int amount = strlen(encoded_msg)+MAX_SOURCE+50;
			char *send = calloc(1, amount);
			if (session->source)
				snprintf(send, amount, "in_reply_to_status_id=%d&source=%s&status=%s", id, session->source, encoded_msg);
			else
				snprintf(send, amount, "in_reply_to_status_id=%d&status=%s", id, encoded_msg);

			ret = gnusocial_api_request(session, send, "statuses/update.xml");
			if (!ret && (session->errormsg = parser_get_error(session->xml)))
				ret = GNUSOCIAL_API_ERROR;
			if (!ret && *session->xml) {
				session->status = calloc(1, sizeof(gnusocial_status_t));
				session->status[0] = parser_get_status(session->xml);
				session->n_status = 1;
			}
			free(send);
			curl_free(encoded_msg);
		}
	}
	return ret;
}

int gnusocial_delete_status(gnusocial_session_t *session, int id)
{
    char flags[16];
    snprintf(flags, sizeof(flags), "id=%d", id);
    int ret = gnusocial_api_request(session, flags, "statuses/destroy.xml");
    if (!ret && (session->errormsg = parser_get_error(session->xml)))
    	    ret = GNUSOCIAL_API_ERROR;
    return ret;
}

int gnusocial_favorite_status(gnusocial_session_t *session, int id)
{
    char flags[16];
    snprintf(flags, sizeof(flags), "id=%d", id);
    int ret = gnusocial_api_request(session, flags, "favorites/create.xml");
    if (!ret && (session->errormsg = parser_get_error(session->xml)))
    	    ret = GNUSOCIAL_API_ERROR;
    return ret;
}

int gnusocial_unfavorite_status(gnusocial_session_t *session, int id)
{
    char flags[16];
    snprintf(flags, sizeof(flags), "id=%d", id);
    int ret = gnusocial_api_request(session, flags, "favorites/destroy.xml");
    if (!ret && (session->errormsg = parser_get_error(session->xml)))
    	    ret = GNUSOCIAL_API_ERROR;
    return ret;
}

int gnusocial_repeat_status(gnusocial_session_t *session, int id, int code)
{
    char url[MAX_URL];
    snprintf(url, MAX_URL, "statuses/retweet/%d.xml", code);
    char id_[32];
    snprintf(id_, sizeof(id_), "id=%d", id);
    int ret = gnusocial_api_request(session, id_, url);
    if (!ret && (session->errormsg = parser_get_error(session->xml)))
    	    ret = GNUSOCIAL_API_ERROR;
    if (!ret && parseXml(session->xml, strlen(session->xml), "status", 6, NULL, 0) < 0) {
    	session->errormsg = calloc(1, GNUSOCIAL_ERROR_SIZE);
        snprintf(session->errormsg, GNUSOCIAL_ERROR_SIZE, "Trying to repeat ID '%d'\n", id);
        ret = GNUSOCIAL_UNKNOWN_ERROR;
    }
    return ret;
}

int gnusocial_search_status(gnusocial_session_t *session, int id)
{
    char xml_doc[32];
    snprintf(xml_doc, sizeof(xml_doc), "statuses/show.xml&id=%d", id);
    int ret = gnusocial_api_request(session, NULL, xml_doc);
    if (!ret && (session->errormsg = parser_get_error(session->xml)))
    	    ret = GNUSOCIAL_API_ERROR;
    if (!ret && *session->xml) {
    	    session->status = calloc(1, sizeof(gnusocial_status_t));
    	    session->status[0] = parser_get_status(session->xml);
    	    session->n_status = 1;
    }
    else if (!ret)
    	    ret = GNUSOCIAL_UNKNOWN_ERROR;
    return ret;
}

int gnusocial_post_status(gnusocial_session_t *session, char *msg)
{
	int ret = GNUSOCIAL_CURL_ERROR;
	/* cURL functionality used just to URIencode the msg */
	CURL *curl = curl_easy_init();
	if(curl) {
		char *encoded_msg = curl_easy_escape(curl, msg, strlen(msg));
		if(encoded_msg) {
			int amount = strlen(encoded_msg)+MAX_SOURCE+15;
			char *send = calloc(1, amount);
			if (session->source)
				snprintf(send, amount, "source=%s&status=%s", session->source, encoded_msg);
			else
				snprintf(send, amount, "status=%s", encoded_msg);

			ret = gnusocial_api_request(session, send, "statuses/update.xml");
			if (!ret && (session->errormsg = parser_get_error(session->xml)))
				ret = GNUSOCIAL_API_ERROR;
			if (!ret && *session->xml) {
				session->status = calloc(1, sizeof(gnusocial_status_t));
				session->status[0] = parser_get_status(session->xml);
				session->n_status = 1;
			}
			free(send);
			curl_free(encoded_msg);
		}
	}
	return ret;
}