/*
 * Copyright (C) 2018 Daniel Martin
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

// #include "gnusocial.h"
 
int parseXml(char *xml_data, int xml_data_size, char *tofind, int tofind_size, char *output, int output_size);

gnusocial_status_t makeStatusFromRawSource(char *raw_data, int data_size);

int FindXmlError(char *xml_data, int xml_data_size);