/*
 * Copyright (C) 2017 Bob Mottram
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
#include <stdio.h>
#include <stdlib.h>

static int download_image_from_url(char * image_url, char * image_filename)
{
    CURL *image;
    CURLcode imgresult;
    FILE *fp;
    int pos, i;
    char extension[8];
    char save_image_filename[512];


    /* get the file extension */
    sprintf(extension,"%s","jpg");
    sprintf(save_image_filename,"%s",image_filename);
    for (pos = strlen(image_url)-1; pos > 1; pos--)
        if (image_url[pos] == '.') break;
    if (pos > 1) {
        sprintf(extension, "%s", &image_url[pos+1]);

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

int get_user_avatar(struct gss_account account, char * username,
                    char * avatar_filename)
{
    char source[512];
    sprintf(source, "screen_name=%s", username);
    char *xml_data = send_to_api(account, source, "users/show.xml");
    char error[512];
    char output[512];
    int xml_data_size = strlen(xml_data);
    struct account_info info;
    if (parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
        info.screen_name[0] = '\0';
        free(xml_data);
        return 1;
    }
    else {
        if (parseXml(xml_data, xml_data_size, "<profile_image_url_profile_size>", 32, output, 512) > 0) {
            download_image_from_url(output, avatar_filename);
            free(xml_data);
            return 0;
        }
    }
    free(xml_data);
    return 2;
}

int get_follow_avatar(struct gss_account account, char * username,
                      char * avatar_filename)
{
    FILE * fp;
    char count[32];
    snprintf(count, 32, "count=%d", 99999);
    char *xml_data = send_to_api(account,count,FRIENDS);
    int xml_data_size = strlen(xml_data);
    char error[512];

    if (parseXml(xml_data, xml_data_size, "<error>", 7, error, 512) > 0) {
        printf("Error: %s\n", error);
    }
    else if (xml_data_size > 0) {
        char screen_name[64];
        char avatar_image_url[512];
        int start_status_point = 0;
        int real_status_point = 0;
        char *array_data;
        array_data = &xml_data[0];
        int i;
        for (i = 0; i < 99999 && (real_status_point+13) < xml_data_size; i++) {
            parseXml(array_data, (xml_data_size-real_status_point), "<screen_name>", 13, screen_name, 64);
            if (strcmp(screen_name, username) == 0) {
                parseXml(array_data, (xml_data_size-real_status_point), "<profile_image_url_profile_size>", 32, avatar_image_url, 512);
                download_image_from_url(avatar_image_url, avatar_filename);
                free(xml_data);
                return 0;
            }
            start_status_point = parseXml(array_data, (xml_data_size-real_status_point), "</user>", 7, "", 0);
            real_status_point += start_status_point;
            array_data = &xml_data[real_status_point];
        }
    }
    else {
        printf("Error: Reading users from '%s://%s/api/%s'\n", account.protocol, account.server, FRIENDS);
    }
    free(xml_data);
    return 1;
}
