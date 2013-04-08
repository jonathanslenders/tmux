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

#include <string.h>
#include <time.h>

#include "tmux.h"

/*
 * Show client message log.
 */

enum cmd_retval	 cmd_show_messages_exec(struct cmd *, struct cmd_q *);
void		 cmd_show_messages_prepare(struct cmd *, struct cmd_q *);

const struct cmd_entry cmd_show_messages_entry = {
	"show-messages", "showmsgs",
	"t:", 0, 0,
	CMD_TARGET_CLIENT_USAGE,
	0,
	NULL,
	NULL,
	cmd_show_messages_exec,
	cmd_show_messages_prepare
};

void
cmd_show_messages_prepare(struct cmd *self, struct cmd_q *cmdq)
{
	struct args		*args = self->args;
	struct cmd_context	*cmd_ctx = cmdq->cmd_ctx;

	cmd_ctx->client = cmd_find_client(cmdq, args_get(args, 't'), 0);
}

enum cmd_retval
cmd_show_messages_exec(unused struct cmd *self, struct cmd_q *cmdq)
{
	struct client		*c;
	struct message_entry	*msg;
	char			*tim;
	u_int			 i;

	if ((c = cmdq->cmd_ctx->client) == NULL)
		return (CMD_RETURN_ERROR);

	for (i = 0; i < ARRAY_LENGTH(&c->message_log); i++) {
		msg = &ARRAY_ITEM(&c->message_log, i);

		tim = ctime(&msg->msg_time);
		*strchr(tim, '\n') = '\0';

		cmdq_print(cmdq, "%s %s", tim, msg->msg);
	}

	return (CMD_RETURN_NORMAL);
}
