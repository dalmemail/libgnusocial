/*
 * Copyright (C) 2017, 2018 Bob Mottram
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

static int _download_image_from_url(char * image_url, char * image_filename)
{
    CURL *image;
    CURLcode imgresult;
    FILE *fp;
    int pos, i;
    char extension[8];
    char save_image_filename[512];


    /* get the file extension */
    snprintf(extension, sizeof(extension), "%s","jpg");
    snprintf(save_image_filename, sizeof(save_image_filename), "%s",image_filename);
    for (pos = strlen(image_url)-1; pos > 1; pos--)
        if (image_url[pos] == '.') break;
    if (pos > 1) {
        snprintf(extension, sizeof(extension), "%s", &image_url[pos+1]);

        for (pos = strlen(image_filename)-1; pos > 1; pos--)
            if (image_filename[pos] == '.') break;
        if (pos > 1) {
            i = pos+1;
            for (pos = 0; pos < strlen(extension); pos++)
                save_image_filename[i++] = extension[pos];
            save_image_filename[i] = 0;
        }
    }

    image = curl_easy_init();
    if (!image) {
        curl_easy_cleanup(image);
        return 1;
    }

    fp = fopen(save_image_filename, "wb");
    if (!fp) {
        curl_easy_cleanup(image);
        return 2;
    }

    curl_easy_setopt(image, CURLOPT_URL, image_url);
    curl_easy_setopt(image, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(image, CURLOPT_WRITEDATA, fp);

    imgresult = curl_easy_perform(image);
    if (imgresult) {
        curl_easy_cleanup(image);
        fclose(fp);
        return 3;
    }

    curl_easy_cleanup(image);
    fclose(fp);
    return 0;
}

int gnusocial_get_user_avatar(gnusocial_session_t *session, char *username,
                       char *avatar_filename)
{
    char flags[256];
    snprintf(flags, sizeof(flags), "screen_name=%s", username);
    int ret = gnusocial_api_request(session, flags, "users/show.xml");

    if (!ret) {
    	char avatar_url[MAX_URL];
        if (parseXml(session->xml, strlen(session->xml),
                        "<profile_image_url_profile_size>", 32, avatar_url, sizeof(avatar_url)) > 0) {
            _download_image_from_url(avatar_url, avatar_filename);
        }
    }
    return ret;
}

int gnusocial_get_follow_avatar(gnusocial_session_t *session, char *username,
                         char *avatar_filename)
{
    FILE *fp;
    char flags[32];
    snprintf(flags, sizeof(flags), "count=%d", 99999);
    int ret = gnusocial_api_request(session, flags, GNUSOCIAL_FRIENDS_LIST);

    if (!ret && *session->xml) {
        char screen_name[64];
        char avatar_image_url[512];
        int start_status_point = 0;
        int real_status_point = 0;
        char *array_data = &session->xml[0];
        int xml_data_size = strlen(session->xml);
        int i;
        for (i = 0; i < 99999 && (real_status_point+13) < xml_data_size; i++) {
            parseXml(array_data, (xml_data_size-real_status_point), "<screen_name>",
                        13, screen_name, 64);
            if (strcmp(screen_name, username) == 0) {
                parseXml(array_data, (xml_data_size-real_status_point),
                            "<profile_image_url_profile_size>", 32, avatar_image_url, 512);
                _download_image_from_url(avatar_image_url, avatar_filename);
                return ret;
            }
            start_status_point =
                parseXml(array_data,
                            (xml_data_size-real_status_point), "</user>", 7, NULL, 0);
            real_status_point += start_status_point;
            array_data = &session->xml[real_status_point];
        }
    }
    else {
    	session->errormsg = calloc(1, GNUSOCIAL_ERROR_SIZE);
        snprintf(session->errormsg, GNUSOCIAL_ERROR_SIZE, "Error: Reading users from '%s://%s/api/%s'\n",
               session->account->protocol, session->account->server, GNUSOCIAL_FRIENDS_LIST);
        ret = GNUSOCIAL_UNKNOWN_ERROR;
    }
    return ret;
}
