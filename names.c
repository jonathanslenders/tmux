/* $Id$ */

/*
 * Copyright (c) 2009 Nicholas Marriott <nicm@users.sourceforge.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>

#include <ctype.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tmux.h"

void window_pane_name_callback(unused int fd, unused short events, void *data);
char	*parse_window_name(const char *);

void
queue_window_pane_name(struct window_pane *wp)
{
	struct timeval	tv;

	tv.tv_sec = 0;
	tv.tv_usec = NAME_INTERVAL * 500L;

	if (event_initialized(&wp->name_timer))
		evtimer_del(&wp->name_timer);
	evtimer_set(&wp->name_timer, window_pane_name_callback, wp);
	evtimer_add(&wp->name_timer, &tv);
}

void
window_pane_name_callback(unused int fd, unused short events, void *data)
{
	struct window_pane	*wp = data;
	char		*name, *wpname;

	if (!wp->automatic_rename) {
		if (event_initialized(&wp->name_timer))
			event_del(&wp->name_timer);
		return;
	}
	queue_window_pane_name(wp);

	name = osdep_get_name(wp->fd, wp->tty);

	if (name == NULL)
		wpname = default_window_pane_name(wp);
	else {
		/*
		 * If tmux is using the default command, it will be a login
		 * shell and argv[0] may have a - prefix. Remove this if it is
		 * present. Ick.
		 */
		if (wp->cmd != NULL && *wp->cmd == '\0' &&
		    name != NULL && name[0] == '-' && name[1] != '\0')
			wpname = parse_window_name(name + 1);
		else
			wpname = parse_window_name(name);
		free(name);
	}
	if (wp->fd == -1) {
		xasprintf(&name, "%s[dead]", wpname);
		free(wpname);
		wpname = name;
	}

	if (wp->name == NULL || strcmp(wpname, wp->name)) {
		window_pane_set_name(wp, wpname);

		/* Redraw status bar if this is the active pane. */
		if (wp->window != wp->window->active)
			server_status_window(wp->window);

		server_redraw_window_borders(wp->window);
	}
	free(wpname);
}

char *
default_window_pane_name(struct window_pane *wp)
{
    return parse_window_name(wp->shell);
}

char *
parse_window_name(const char *in)
{
	char	*copy, *name, *ptr;

	name = copy = xstrdup(in);
	if (strncmp(name, "exec ", (sizeof "exec ") - 1) == 0)
		name = name + (sizeof "exec ") - 1;

	while (*name == ' ')
		name++;
	if ((ptr = strchr(name, ' ')) != NULL)
		*ptr = '\0';

	if (*name != '\0') {
		ptr = name + strlen(name) - 1;
		while (ptr > name && !isalnum((u_char)*ptr))
			*ptr-- = '\0';
	}

	if (*name == '/')
		name = basename(name);
	name = xstrdup(name);
	free(copy);
	return (name);
}
