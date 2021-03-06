/*
 * Copyright (C) 2018 Daniel Martín
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

#include <stdlib.h>
#include "gnusocial.h"
#include "mem.h"

gnusocial_session_t *gnusocial_session_create(void)
{
	return calloc(1, sizeof(gnusocial_session_t));
}

void gnusocial_session_free(gnusocial_session_t *session)
{
	if (!session)
		return;

	xfree(session->account);
	xfree(session->status);
	xfree(session->groups);
	xfree(session->accounts);
	xfree(session->errormsg);
	xfree(session->xml);
	xfree(session->source);

	xfree(session);
}

char *gnusocial_session_xml(gnusocial_session_t *session)
{
	return session->xml;
}

char *gnusocial_session_error_reason(gnusocial_session_t *session)
{
	return session->errormsg;
}

void gnusocial_session_reset(gnusocial_session_t *session)
{
	if (!session)
		return;

	xfree(session->status);
	session->n_status = 0;
	xfree(session->groups);
	session->n_groups = 0;
	xfree(session->accounts);
	session->n_accounts = 0;
	xfree(session->errormsg);
	xfree(session->xml);
}

void gnusocial_session_set_source(gnusocial_session_t *session, const char *source)
{
	xfree(session->source);

	session->source = calloc(1, MAX_SOURCE);
	memcpy(session->source, source, MAX_SOURCE);
}

void gnusocial_session_remove_source(gnusocial_session_t *session)
{
		xfree(session->source);

	session->source = NULL;
}