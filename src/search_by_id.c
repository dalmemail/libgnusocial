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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

gnusocial_status_t search_by_id(gnusocial_account_t account, int id, int *result)
{
    char xml_doc[32];
    snprintf(xml_doc, 32, "statuses/show.xml&id=%d", id);
    char *xml_data = send_to_api(account,NULL,xml_doc);
    int xml_data_size = strlen(xml_data);
    gnusocial_status_t status_by_id;
    if (FindXmlError(xml_data, xml_data_size) < 0 && parseXml(xml_data, xml_data_size, "</status>", 9, NULL, 0) > 0) {
        status_by_id = makeStatusFromRawSource(xml_data, xml_data_size);
        *result = 0;
    }
    free(xml_data);
    return status_by_id;
}
