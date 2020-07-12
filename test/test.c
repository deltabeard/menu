#include "../menu.h"
#include "minctest.h"

#define arraysz(x) sizeof(x)/sizeof(*x)

void increment(void *p)
{
	int *i = p;
	(*i)++;
}

void decrement(void *p)
{
	int *i = p;
	(*i)--;
}

void test_func(void)
{
	int i = 0;
	unsigned l;

	menu_ctx *m = menu_alloc(malloc);
	struct menu_item_s items[] = {
		{ "Increment int", "help text", MENU_EXEC_FUNC, { NULL }},
		{ "Decrement int", "help text", MENU_EXEC_FUNC, { NULL }},
	};
	items[0].param.exec_func.ctx = &i;
	items[0].param.exec_func.func = increment;
	items[1].param.exec_func.ctx = &i;
	items[1].param.exec_func.func = decrement;

	m = menu_create(m, NULL, "Top menu", "Select an item", arraysz(items), items);
	m = menu_instruct(m, MENU_INSTR_EXEC_ITEM);
	m = menu_instruct(m, MENU_INSTR_EXEC_ITEM);
	m = menu_instruct(m, MENU_INSTR_EXEC_ITEM);
	lequal(i, 3);
	m = menu_instruct(m, MENU_INSTR_NEXT_ITEM);
	m = menu_instruct(m, MENU_INSTR_EXEC_ITEM);
	lequal(i, 2);
	m = menu_instruct(m, MENU_INSTR_PREV_ITEM);
	m = menu_instruct(m, MENU_INSTR_EXEC_ITEM);
	lequal(i, 3);

	/* Test limits. */
	for(l = 20; l > 0; l--)
		m = menu_instruct(m, MENU_INSTR_NEXT_ITEM);

	/* Should be set to decrement. */
	m = menu_instruct(m, MENU_INSTR_EXEC_ITEM);
	lequal(i, 2);
}

int main(void)
{
	test_func();
	lresults();
	return lfails != 0;
}
