/**
 * Copyright (C) 2020 by Mahyar Koshkouei <mk@deltabeard.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include "../menu.h"

#include <stdlib.h>
#include <stdio.h>

#define arraysz(x) sizeof(x)/sizeof(*x)

void print_message(void *ctx)
{
	const char *msg = ctx;
	puts(msg);
}

int main(void)
{
	menu_ctx root;
	int quit = 0;
	static const char *msg = "Hello World!";

	menu_item root_items[] = {
		{ "Print message", "This prints a message.", MENU_EXEC_FUNC, { NULL }},
		{ "Quit", "Quit this program.", MENU_SET_VAL, { NULL }}
	};

	root_items[0].param.exec_func.ctx = (void *)msg;
	root_items[0].param.exec_func.func = print_message;

	root_items[1].param.set_val.val = 1;
	root_items[1].param.set_val.set = &quit;

	menu_init(&root, NULL, "Root Menu", NULL, arraysz(root_items), root_items);

	while(quit == 0)
	{
		unsigned long i;
		char in[32];
		int sel;

		printf("%s\n", root.title);
		printf("There are %lu options:\n", root.items_nmemb);

		for(i = 0; i < arraysz(root_items); i++)
			printf("%lu: %s\n", i, root_items[i].name);

		printf("Enter an option number: ");
		fflush(stdout);

		fgets(in, sizeof(in), stdin);
		sel = atoi(in);

		MENU_SELECT_ITEM((&root), (unsigned)sel);
		printf("You selected option %d\n", sel);
		menu_instruct(&root, MENU_INSTR_EXEC_ITEM);
	}

	puts("Quitting");

	return EXIT_SUCCESS;
}
