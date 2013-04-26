/* $Id$ */

/*
 * Copyright (c) 2007 Nicholas Marriott <nicm@users.sourceforge.net>
 * Copyright (c) 2013 Jonathan Slenders <jonathan_s@users.sourceforge.net>
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

#include <stdlib.h>

#include "tmux.h"

/*
 * Rename a pane.
 */

enum cmd_retval	 cmd_rename_pane_exec(struct cmd *, struct cmd_q *);

const struct cmd_entry cmd_rename_pane_entry = {
	"rename-pane", "renamep",
	"t:", 1, 1,
	CMD_TARGET_WINDOW_USAGE " new-name",
	0,
	NULL,
	NULL,
	cmd_rename_pane_exec
};


enum cmd_retval
cmd_rename_pane_exec(struct cmd *self, struct cmd_q *cmdq)
{
	struct args	*args = self->args;
	struct window_pane	*wp;
	struct winlink		*wl;

	if ((wl = cmd_find_pane(cmdq, args_get(args, 't'), NULL, &wp)) == NULL)
		return (CMD_RETURN_ERROR);

	wp->automatic_rename = 0;
	window_pane_set_name(wp, args->argv[0]);

	server_redraw_window(wl->window);

	return (CMD_RETURN_NORMAL);
}
