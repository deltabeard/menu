#include "../menu.h"
#include "minctest.h"

#include <stdlib.h>

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

/* Using calloc to appease valgrind uninitialised usage errors.
 * Using malloc should be fine however: "menu_alloc(malloc)" */
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

void test_submenu(void)
{
	menu_ctx *top, *fruit, *veg, *meat, *current;
	static struct menu_item_s top_items[] = {
		{ "Fruits",	NULL, MENU_SUB_MENU, { NULL }},
		{ "Vegetables",	NULL, MENU_SUB_MENU, { NULL }},
		{ "Meat",	NULL, MENU_SUB_MENU, { NULL }}
	};
	static struct menu_item_s f_items[] = {
		{ "Orange",	NULL, MENU_SUB_MENU, { NULL }},
		{ "Apple",	NULL, MENU_SUB_MENU, { NULL }}
	};
	static struct menu_item_s v_items[] = {
		{ "Carrot",	NULL, MENU_SUB_MENU, { NULL }},
		{ "Broccoli",	NULL, MENU_SUB_MENU, { NULL }}
	};
	static struct menu_item_s m_items[] = {
		{ "Lamb",	NULL, MENU_SUB_MENU, { NULL }},
		{ "Beef",	NULL, MENU_SUB_MENU, { NULL }}
	};

	/* Initialise menus. */
	top = menu_alloc(menu_calloc);
	fruit = menu_alloc(menu_calloc);
	veg = menu_alloc(menu_calloc);
	meat = menu_alloc(menu_calloc);

	/* Set items in top menu. */
	top_items[0].param.sub_menu = fruit;
	top_items[1].param.sub_menu = veg;
	top_items[2].param.sub_menu = meat;
	top = menu_create(top, NULL, "Food", "Select an item",
			arraysz(top_items), top_items);

	fruit = menu_create(fruit, top, "Food", "Select an item",
			arraysz(f_items), f_items);
	veg = menu_create(veg, top, "Food", "Select an item",
			arraysz(v_items), v_items);
	meat = menu_create(meat, top, "Food", "Select an item",
			arraysz(m_items), m_items);

	current = menu_instruct(top, MENU_INSTR_EXEC_ITEM);
	lok(current == fruit);

	/* If the current selection is of sub-menu type, but the sub-menu is
	 * set to NULL, then the sub-menu is not executed.
	 * So basically, it is a no-op instruction. You should not do this in
	 * production, as the user would expect something to happen when
	 * selecting an item in a menu.
	 * Therefore, before presenting the menu to the user, make sure that
	 * all sub-menus are initialised to other menus. */
	current = menu_instruct(top, MENU_INSTR_EXEC_ITEM);
	lok(current == fruit);

	current = menu_instruct(top, MENU_INSTR_PARENT_MENU);
	lok(current == top);

	menu_delete(top, free);
	menu_delete(fruit, free);
	menu_delete(veg, free);
	menu_delete(meat, free);
}

int main(void)
{
	test_func();
	test_setval();
	test_submenu();
	lresults();
	return lfails != 0;
}
