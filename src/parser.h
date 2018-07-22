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

/**
 * @brief Parses a concrete tag into a XML data content
 * @param xml_data Pointer to the XML file content
 * @param xml_data_size Size (length) of that content
 * @param tofind Pointer to the XML tag e.g. "<text>"
 * @param tofind_size Length of the tag string
 * @param output Pointer to the string where the parsed content will be stored
 * @param output_size Size of that string
 * @return The last char written into output position in xml_data or -1 if tag not found
 */

int parseXml(char *xml_data, int xml_data_size, char *tofind, int tofind_size, char *output, int output_size);

/**
 * @brief Parses XML content into a gnusocial_status_t
 * @param xml_data Pointer to the XML file content
 * @return A gnusocial_status_t
 */
gnusocial_status_t parser_get_status(char *xml_data);

/**
 * @brief Parses XML content into a gnusocial_group_info_t
 * @param xml_data Pointer to the XML file content
 * @return A gnusocial_group_info_t
 */

gnusocial_group_info_t parser_get_group_info(char *xml_data);

/**
 * @brief Parses XML content into a gnusocial_account_info_t
 * @param xml_data Pointer to the XML file content
 * @return A gnusocial_account_info_t
 */

gnusocial_account_info_t parser_get_account_info(char *xml_data);

/**
 * @brief Parses XML content to find an error
 * @param xml_data Pointer to the XML file content
 * @return A pointer to the error string
 */

char *parser_get_error(char *xml_data);