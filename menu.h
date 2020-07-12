/**
 * Create and manipulate a menu system.
 *
 * Initialise menu
 * Configure menu
 * Browse through menu
 * Select menu item
 */

#pragma once
#include <stdlib.h>

typedef struct menu_ctx_s menu_ctx;

enum menu_instruction_e {
	MENU_INSTR_NO_OP = 0,

	/* Go back to the previous item.
	 * Could be used when user presses UP. */
	MENU_INSTR_PREV_ITEM,

	/* Go to next item in menu.
	 * Could be used when user presses DOWN. */
	MENU_INSTR_NEXT_ITEM,

	/* Go to parent menu if one exists.
	 * Could be used when user presses BACKSPACE. */
	MENU_INSTR_PARENT_MENU,

	/* Execute item operation.
	 * Could be used when user presses ENTER. */
	MENU_INSTR_EXEC_ITEM
};

struct menu_item_s
{
	const char *name;
	const char *help;
	enum menu_op_e
	{
		/* Opens a sub menu. */
		MENU_SUB_MENU,

		/* Executes a function. */
		MENU_EXEC_FUNC,

		/* Sets the value of an integer. */
		MENU_SET_VAL
	} op;

	union param_u
	{
		/* Pointer to sub menu. */
		menu_ctx *sub_menu;

		/* Pointer to function to execute if item selected. */
		struct exec_func_s
		{
			void *ctx;
			void (*func)(void *ctx);
		} exec_func;

		/* Pointer to integer to set. */
		struct set_val_s
		{
			int *val;
		} set_val;
	} param;
};

menu_ctx *menu_alloc(void *(*menu_malloc)(size_t size));
menu_ctx *menu_create(menu_ctx *menu, menu_ctx *parent, const char *title,
		const char *help, unsigned long items_nmemb,
		struct menu_item_s *items);
void menu_delete(menu_ctx *ctx, void (*menu_free)(void *ptr));
menu_ctx *menu_set_items(menu_ctx *menu, unsigned long nmemb,
		struct menu_item_s *items);
menu_ctx *menu_instruct(menu_ctx *ctx, enum menu_instruction_e instr);
