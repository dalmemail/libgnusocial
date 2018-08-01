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

const char *gnusocial_error[] = {
 	"Request Succeed",
 	"Protocol is too long or too short (null)",
 	"Username is too long or too short (null)",
 	"Password is too long or too short (null)",
 	"The proxy given is too long",
 	"Trying to allocate memory: NULL was returned",
 	"Session is not initialized: No request can be made",
 	"Account is not initialized: No request can be made",
 	"cURL ERROR (libcurl)",
 	"SERVER API ERROR",
 	"SERVER API ERROR: Incomplete XML content: Missing expected tag",
 	"Invalid account credentials",
 	"Unknown Error"
};

const char *gnusocial_get_error(int error_code)
{
	if (error_code < 0)
		error_code *= -1;
	else
		error_code = 0;
		
	return gnusocial_error[error_code];
}