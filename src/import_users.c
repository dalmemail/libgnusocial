/*
 * Copyright (C) 2017, 2018 Bob Mottram.
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

void gs_import_users(gnusocial_account_t account, char *filename)
{
    FILE * fp;
    char send[256], line[256];
    int i;
    char *xml_data;
    const char field_separator = ',';

    fp = fopen(filename,"r");
    if (fp==0) {
        printf("File not found\n");
        return;
    }

    while (!feof(fp))
        if (fgets(line , 255 , fp) != NULL ) {
            /* get the last field */
            i = strlen(line)-1;
            while ((i > 1) && (line[i] != field_separator))
                i--;
            if (line[i] != field_separator)
                continue;
            i++;

            snprintf(send, 255, "ostatus_uri=%s", (char*)&line[i]);
            printf("%s\n", send);
            xml_data = gs_send_to_api(account, send, "friendships/create.xml");
            gs_FindXmlError(xml_data, strlen(xml_data));
            free(xml_data);
        }

    fclose(fp);
    printf("Done\n", send);
}
