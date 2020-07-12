#include <stdlib.h>
#include <string.h>

#include "menu.h"

#if 0
enum item_selection_type_e {
	/* An array of with nmemb number of
	 * const char pointers. */
	ITEM_SEL_STRINGS,

	/* An unsigned value for enable (1) and
	 * disable (0). nmemb is ignored. */
	ITEM_SEL_EN_DIS,

	/* An array of three 32-bit signed integers for
	 * start, step, end. */
	ITEM_SEL_INT_RANGE,

	/* An array of 32-bit signed integers. The first
	 * integer is the length of the array, and
	 * should not be considered a valid input value.
	 */
	ITEM_SEL_INT_SPECIFIC
};
#endif

struct menu_ctx_s
{
	struct menu_ctx_s *parent;
	const char *title;
	const char *help;
	unsigned long item_selected;
	unsigned long items_nmemb;
	const struct menu_item_s *items;
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

menu_ctx *menu_alloc(void *(*menu_malloc)(size_t size))
{
	return menu_malloc(sizeof(menu_ctx));
}

menu_ctx *menu_create(menu_ctx *menu, menu_ctx *parent, const char *title,
		const char *help, unsigned long items_nmemb,
		struct menu_item_s *items)
{
	menu->parent = parent;
	menu->title = title;
	menu->help = help;
	menu->items_nmemb = items_nmemb;
	menu->items = items;
	return menu;
}

void menu_delete(menu_ctx *ctx, void (*menu_free)(void *ptr))
{
	menu_free(ctx);
	ctx = NULL;
}

/**
 * Add an item to a given menu. First item shown first in menu.
 */
menu_ctx *menu_set_items(menu_ctx *menu, unsigned long nmemb,
		struct menu_item_s *items)
{
	menu->items_nmemb = nmemb;
	menu->items = items;
	return menu;
}

menu_ctx *menu_instruct(menu_ctx *ctx, enum menu_instruction_e instr)
{
	menu_ctx *ret = ctx;
	switch(instr)
	{
		case MENU_INSTR_NO_OP:
			break;

		case MENU_INSTR_PREV_ITEM:
			if(ctx->item_selected > 0)
				ctx->item_selected--;

			break;

		case MENU_INSTR_NEXT_ITEM:
			if(ctx->item_selected < (ctx->items_nmemb - 1))
				ctx->item_selected++;

			break;
		case MENU_INSTR_PARENT_MENU:
			if(ctx->parent != NULL)
				ret = ctx->parent;

			break;

		case MENU_INSTR_EXEC_ITEM:
		{
			const struct menu_item_s *item =
				ctx->items + ctx->item_selected;

			switch(item->op)
			{
				case MENU_SUB_MENU:
					ret = item->param.sub_menu;
					break;

				case MENU_EXEC_FUNC:
				{
					void *p = item->param.exec_func.ctx;
					item->param.exec_func.func(p);
					break;
				}

				case MENU_SET_VAL:
					*item->param.set_val.val = 0;
					break;
			}
			break;
		}
	}

	return ret;
}
