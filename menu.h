/**
 * Create and manipulate a menu system.
 *
 * Initialise menu
 * Configure menu
 * Browse through menu
 * Select menu item
 */

#pragma once

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

menu_ctx *menu_instruct(menu_ctx *ctx, enum menu_instruction_e instr);
