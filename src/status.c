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

void gnusocial_reply_status(gnusocial_account_t account, int id, char *msg)
{
        /* cURL functionality used just to URIencode the msg */
        CURL *curl = curl_easy_init();
    if(curl) {
                char *encoded_msg = curl_easy_escape(curl, msg, strlen(msg));
        if(encoded_msg) {
                /* margin to fit the ID is included */
                        int amount = 68+strlen(encoded_msg);
            char *send = malloc(amount);
            snprintf(send, amount,
                     "in_reply_to_status_id=%d&source=GnuSocialShell&status=%s",
                     id, encoded_msg);
            /*if (loglevel >= LOG_DEBUG) { // OK?
                    fprintf(stderr,
                            "in_reply_to_status_id=%d&source=GnuSocialShell&status=%s\n",
                            id, encoded_msg);
            }*/
            // send[sizeof(send)-1] = '\0'; // snprintf does that too
            char *xml_data = gnusocial_api_request(account, send, "statuses/update.xml");
            int xml_data_size = strlen(xml_data);
            if (FindXmlError(xml_data, strlen(xml_data)) < 0 &&
                parseXml(xml_data, xml_data_size, "</status>", 9, NULL, 0) > 0) {
                /*struct gnusocial_status posted_status;
                posted_status = makeStatusFromRawSource(xml_data, xml_data_size);
                print_status(posted_status);*/
            }
            free(xml_data);
            free(send);
                curl_free(encoded_msg);
        }
    }
}

void gnusocial_delete_status(gnusocial_account_t account, int id)
{
    char send[16];
    snprintf(send, 16, "id=%d", id);
    char *xml_data = gnusocial_api_request(account, send, "statuses/destroy.xml");
    FindXmlError(xml_data, strlen(xml_data));
    free(xml_data);
}

void gnusocial_favorite_status(gnusocial_account_t account, int id)
{
    char send[16];
    snprintf(send, 16, "id=%d", id);
    char *xml_data = gnusocial_api_request(account, send, "favorites/create.xml");
    FindXmlError(xml_data, strlen(xml_data));
    free(xml_data);
}

void gnusocial_unfavorite_status(gnusocial_account_t account, int id)
{
    char send[16];
    snprintf(send, 16, "id=%d", id);
    char *xml_data = gnusocial_api_request(account, send, "favorites/destroy.xml");
    FindXmlError(xml_data, strlen(xml_data));
    free(xml_data);
}

void gnusocial_repeat_status(gnusocial_account_t account, int id, int code)
{
    char url[MAX_URL];
    snprintf(url, MAX_URL, "statuses/retweet/%d.xml", code);
    char id_[32];
    snprintf(id_, 32, "id=%d", id);
    char *xml_data = gnusocial_api_request(account,id_,url);
    int xml_data_size = strlen(xml_data);
    char error[512];
    if (parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
    }
    else if (parseXml(xml_data, xml_data_size, "status", 6, "", 0) < 0) {
        printf("Error: Trying to repeat ID '%d'\n", id);
    }
    free(xml_data);
}

gnusocial_status_t gnusocial_search_status(gnusocial_account_t account, int id, int *result)
{
    char xml_doc[32];
    snprintf(xml_doc, 32, "statuses/show.xml&id=%d", id);
    char *xml_data = gnusocial_api_request(account,NULL,xml_doc);
    int xml_data_size = strlen(xml_data);
    gnusocial_status_t status_by_id;
    if (FindXmlError(xml_data, xml_data_size) < 0 &&
        parseXml(xml_data, xml_data_size, "</status>", 9, NULL, 0) > 0) {
        status_by_id = makeStatusFromRawSource(xml_data, xml_data_size);
        *result = 0;
    }
    free(xml_data);
    return status_by_id;
}

void gnusocial_post_status(gnusocial_account_t account, char *msg)
{
        /* cURL functionality used just to URIencode the msg */
        CURL *curl = curl_easy_init();
    if(curl) {
                char *encoded_msg = curl_easy_escape(curl, msg, strlen(msg));
        if(encoded_msg) {
                        int amount = 31+strlen(encoded_msg);
            char *send = malloc(amount);
            snprintf(send, amount, "source=GnuSocialShell&status=%s", encoded_msg);
            /*if (loglevel >= LOG_DEBUG) { // OK?
                    fprintf(stderr, "source=GnuSocialShell&status=%s", encoded_msg);
            }*/
            char *xml_data = gnusocial_api_request(account, send, "statuses/update.xml");
            int xml_data_size = strlen(xml_data);
            if (FindXmlError(xml_data, strlen(xml_data)) < 0 &&
                parseXml(xml_data, xml_data_size, "</status>", 9, NULL, 0) > 0) {
                /*struct gnusocial_status posted_status;
                posted_status = makeStatusFromRawSource(xml_data, xml_data_size);
                print_status(posted_status);*/
            }
            free(xml_data);
            free(send);
            curl_free(encoded_msg);
        }
    }
}