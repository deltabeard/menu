#include <stdlib.h>
#include <string.h>

/**
 * Create and manipulate a menu system.
 *
 * Initialise menu
 * Configure menu
 * Browse through menu
 * Select menu item
 */

struct menu_ctx_s
{
	struct menu_ctx_s *parent;
	const char *menu_title;
	const char *menu_help;
	unsigned long items_nmemb;
	struct menu_item_s *items;
};
typedef struct menu_ctx_s menu_ctx;

struct menu_item_s
{
	const char *item_name;
	const char *item_help;
	enum menu_op_e
	{
		/* Opens a sub menu. */
		MENU_SUB_MENU,

		/* Executes a function. */
		MENU_OP_EXE_FUNC
	} op;

	union
	{
		menu_ctx *sub_menu;
		struct
		{
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
				ITEM_SEL_INT_SPECIFIC,
			} item_type;
			const void *options;
			unsigned long nmemb_options;
			const void (*menu_func_op)(void *ctx, void *selected_option);
		} opdat;
	} itemdat;
};

menu_ctx *menu_init(const char *root_menu_title, const char *help)
{
	menu_ctx *ctx = malloc(sizeof(menu_ctx));

	if(ctx == NULL)
		return NULL;

	ctx->parent = NULL;
	ctx->menu_title = root_menu_title;
	ctx->menu_help = help;
	ctx->items_nmemb = 0;
	ctx->items = NULL;
	return ctx;
}

/**
 * Add an item to a given menu. First item shown first in menu.
 */
menu_ctx *menu_add_item(menu_ctx *ctx, struct menu_item_s *newitem)
{
	struct menu_item_s *item = ctx->items + ctx->items_nmemb;

	ctx->items_nmemb++;
	ctx = realloc(ctx->items, ctx->items_nmemb * sizeof(*ctx->items));
	if(ctx == NULL)
		return NULL;

	*item = *newitem;
	return ctx;
}

/**
 * Used to recreate menu.
 */
menu_ctx *menu_del_items(menu_ctx *ctx)
{
	free(ctx->items);
	ctx->items = NULL;
	ctx->items_nmemb = 0;
	return ctx;
}

struct menu_item_s *menu_add_submenu(struct menu_item_s *item, menu_ctx *submenu)
{
	item->itemdat.sub_menu = submenu;
	return item;
}

void menu_exit(menu_ctx *ctx)
{
	free(ctx);
	ctx = NULL;
}
