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

/* Using calloc to appease valgrind uninitialised usage errors. */
void *menu_calloc(size_t sz)
{
	return calloc(sz, 1);
}

void test_func(void)
{
	int i = 0;
	unsigned l;
	menu_ctx *m = NULL;
	static struct menu_item_s items[] = {
		{ "Increment int", "help text", MENU_EXEC_FUNC, { NULL }},
		{ "Decrement int", "help text", MENU_EXEC_FUNC, { NULL }},
	};

	m = menu_alloc(menu_calloc);
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

	menu_delete(m, free);
}

void test_setval(void)
{
	int i = 0;
	menu_ctx *m = NULL;
	static struct menu_item_s items[] = {
		{ "Set int to 9001", NULL, MENU_SET_VAL, { NULL }},
		{ "Set int to 42", NULL, MENU_SET_VAL, { NULL }},
	};

	m = menu_alloc(menu_calloc);
	items[0].param.set_val.set = &i;
	items[0].param.set_val.val = 9001;
	items[1].param.set_val.set = &i;
	items[1].param.set_val.val = 42;

	m = menu_create(m, NULL, "Top menu", "Select an item", arraysz(items), items);
	lequal(i, 0);
	m = menu_instruct(m, MENU_INSTR_EXEC_ITEM);
	lequal(i, 9001);
	m = menu_instruct(m, MENU_INSTR_NEXT_ITEM);
	m = menu_instruct(m, MENU_INSTR_EXEC_ITEM);
	lequal(i, 42);
	m = menu_instruct(m, MENU_INSTR_NEXT_ITEM);
	m = menu_instruct(m, MENU_INSTR_EXEC_ITEM);
	lequal(i, 42);
	m = menu_instruct(m, MENU_INSTR_PREV_ITEM);
	m = menu_instruct(m, MENU_INSTR_EXEC_ITEM);
	lequal(i, 9001);

	menu_delete(m, free);
}

int main(void)
{
	test_func();
	test_setval();
	lresults();
	return lfails != 0;
}
