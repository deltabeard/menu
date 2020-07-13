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
#include "sdl2_picofont.h"

#include <SDL2/SDL.h>

#include <stdlib.h>
#include <stdio.h>

/* This was used to create menu.gif for the project readme. */
//#define RECORD_FRAMES 1

struct colours_s {
	int r, g, b;
};

void reset_colour(void *p)
{
	struct colours_s *c = p;
	c->b = 0;
	c->g = 0;
	c->r = 0;
}

int main(int argc, char *argv[])
{
	SDL_Window *win;
	SDL_Renderer *ren;
	font_ctx *font;
	menu_ctx root, rm, gm, bm;
	menu_ctx *current = &root;
	struct colours_s c = { 0 };
	int ret = EXIT_SUCCESS;
	int run = 1;
	menu_item root_items[] = {
		{ "Red", NULL, MENU_SUB_MENU, .param.sub_menu = &rm },
		{ "Green", NULL, MENU_SUB_MENU, .param.sub_menu = &gm },
		{ "Blue", NULL, MENU_SUB_MENU, .param.sub_menu = &bm },
		{ "Reset", "Reset background colour to black", MENU_EXEC_FUNC,
			.param.exec_func = { &c, reset_colour }},
		{ "Exit", "Exit example application", MENU_SET_VAL,
			.param.set_val = { 0, &run }}
	};
	menu_item r_items[] = {
		{ "0", NULL, MENU_SET_VAL, .param.set_val = { 0, &c.r }},
		{ "1", NULL, MENU_SET_VAL, .param.set_val = { 1, &c.r }},
		{ "2", NULL, MENU_SET_VAL, .param.set_val = { 2, &c.r }},
		{ "3", NULL, MENU_SET_VAL, .param.set_val = { 3, &c.r }}
	};
	menu_item g_items[] = {
		{ "0", NULL, MENU_SET_VAL, .param.set_val = { 0, &c.g }},
		{ "1", NULL, MENU_SET_VAL, .param.set_val = { 1, &c.g }},
		{ "2", NULL, MENU_SET_VAL, .param.set_val = { 2, &c.g }},
		{ "3", NULL, MENU_SET_VAL, .param.set_val = { 3, &c.g }}
	};
	menu_item b_items[] = {
		{ "0", NULL, MENU_SET_VAL, .param.set_val = { 0, &c.b }},
		{ "1", NULL, MENU_SET_VAL, .param.set_val = { 1, &c.b }},
		{ "2", NULL, MENU_SET_VAL, .param.set_val = { 2, &c.b }},
		{ "3", NULL, MENU_SET_VAL, .param.set_val = { 3, &c.b }}
	};

	(void) argc;
	(void) argv;

	ret = SDL_Init(SDL_INIT_VIDEO);
	if(ret != 0)
		goto out;

	ret = SDL_CreateWindowAndRenderer(320, 240, 0, &win, &ren);
	if(ret != 0)
		goto out;

	font = FontStartup(ren);
	if(font == NULL)
		goto out;

	menu_init(&root, NULL, "Change background colour",
			"Use the options below to change the background colour",
			SDL_arraysize(root_items), root_items);
	menu_init(&rm, &root, "Set red intensity", NULL,
			SDL_arraysize(r_items), r_items);
	menu_init(&gm, &root, "Set green intensity", NULL,
			SDL_arraysize(g_items), g_items);
	menu_init(&bm, &root, "Set blue intensity", NULL,
			SDL_arraysize(b_items), b_items);

	SDL_SetRenderDrawColor(ren, c.r, c.g, c.b, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(ren);

	while (run) {
		SDL_Event ev;
		SDL_Rect floc = { .w = 1, .h = 1 };
		static int pressed = 1;

		while (SDL_PollEvent(&ev) != 0)
		{
			if (ev.type == SDL_QUIT)
				run = 0;
			else if(ev.type != SDL_KEYDOWN)
				continue;

			if(ev.key.repeat)
				continue;

			pressed = 1;

			switch(ev.key.keysym.sym)
			{
			case SDLK_s:
			case SDLK_DOWN:
				current = menu_instruct(current, MENU_INSTR_NEXT_ITEM);
				break;

			case SDLK_w:
			case SDLK_UP:
				current = menu_instruct(current, MENU_INSTR_PREV_ITEM);
				break;

			case SDLK_d:
			case SDLK_SPACE:
			case SDLK_RIGHT:
			case SDLK_RETURN:
				current = menu_instruct(current, MENU_INSTR_EXEC_ITEM);
				break;

			case SDLK_a:
			case SDLK_LEFT:
				current = menu_instruct(current, MENU_INSTR_PARENT_MENU);
				break;
			}
		};

		if(pressed == 0)
		{
			SDL_RenderPresent(ren);
			SDL_Delay(10);
			continue;
		}

		SDL_SetRenderDrawColor(ren, c.r * 64, c.g * 64, c.b * 64, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(ren);
		SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
		FontPrintToRenderer(font, current->title, &floc);
		floc.y += 15;
		floc.x += 18;

		for(unsigned i = 0; i < current->items_nmemb; i++)
		{
			floc.y += 15;
			if(i == current->item_selected)
			{
				int prev = floc.x;
				floc.x = 0;
				FontPrintToRenderer(font, ">", &floc);
				floc.x = prev;
			}

			FontPrintToRenderer(font, current->items[i].name, &floc);
		}

#if defined(RECORD_FRAMES)
		{
			static SDL_Surface *surf = NULL;
			static unsigned long num = 0;
			char name[32];

			if(surf == NULL)
			{
				surf = SDL_CreateRGBSurfaceWithFormat(0,
						320, 240, 32,
						SDL_PIXELFORMAT_RGBA32);
			}

			SDL_RenderReadPixels(ren, NULL, SDL_PIXELFORMAT_RGBA32,
					surf->pixels, surf->pitch);

			SDL_snprintf(name, sizeof(name), "%08lu.bmp", num);
			num++;
			SDL_SaveBMP(surf, name);
			SDL_Delay(100);
		}
#endif

		SDL_RenderPresent(ren);
		SDL_Delay(10);
		pressed = 0;
	}

	FontExit(font);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);

out:
	SDL_Quit();
	return ret;
}
