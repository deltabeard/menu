/**
 * This file is released under the UNLICENSE license, the terms of which are
 * provided within the LICENSE file.
 */

#include <SDL2/SDL.h>

#include "sdl2_picofont.h"

#if defined(PICOFONT_5x8)
#include "5x8.h"
#elif defined(PICOFONT_9x15)
#include "9x15.h"
#else
#include "9x15.h"
#warning "No font specified. Using 9x15 font by default."
#endif

struct font_ctx_s
{
	SDL_Texture *tex;
	SDL_Renderer *rend;
};

font_ctx *FontStartup(SDL_Renderer *renderer)
{
	const SDL_Colour colours[2] =
	{
		{ .r = 0x00, .g = 0x00, .b = 0x00, .a = 0x00 }, // BG
		{ .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF }  // FG
	};
	SDL_Surface *bmp_surf;
	font_ctx *ctx = SDL_malloc(sizeof(font_ctx));
	Uint8 *pixels = SDL_malloc(FONT_BITMAP_SIZE);

	SDL_assert(renderer != NULL);

	if(ctx == NULL || pixels == NULL)
	{
		SDL_SetError("Unable to allocate memory.");
		goto err;
	}

	SDL_memcpy(pixels, bitmap_font, FONT_BITMAP_SIZE);
	bmp_surf = SDL_CreateRGBSurfaceFrom(pixels,
	                                    FONT_BITMAP_WIDTH,
	                                    FONT_BITMAP_HEIGHT,
	                                    1, FONT_PITCH, 0, 0, 0, 0);
	if(bmp_surf == NULL)
		goto err;

	if(SDL_SetPaletteColors(bmp_surf->format->palette, colours, 0, 2) < 0)
	{
		SDL_FreeSurface(bmp_surf);
		goto err;
	}

	if(SDL_SetColorKey(bmp_surf, SDL_TRUE, 0) < 0)
	{
		SDL_FreeSurface(bmp_surf);
		goto err;
	}

	ctx->tex = SDL_CreateTextureFromSurface(renderer, bmp_surf);
	if(ctx->tex == NULL)
	{
		SDL_FreeSurface(bmp_surf);
		goto err;
	}

	ctx->rend = renderer;
	SDL_FreeSurface(bmp_surf);

out:
	SDL_free(pixels);
	return ctx;

err:
	SDL_free(ctx);
	ctx = NULL;
	goto out;
}

void FontDrawSize(const size_t text_len, int *w, int *h)
{
	*w = text_len * FONT_CHAR_WIDTH;
	*h = FONT_CHAR_HEIGHT;
}

int FontPrintToRenderer(font_ctx *const ctx, const char *text,
			const SDL_Rect *dstscale)
{
	SDL_Rect font_rect, screen_rect;
	SDL_Rect dst;

	SDL_assert(ctx != NULL);
	SDL_assert(text != NULL);

	if(dstscale == NULL)
	{
		dst.w = 1;
		dst.h = 1;
		dst.x = 0;
		dst.y = 0;
	}
	else
		dst = *dstscale;

	font_rect.w = FONT_CHAR_WIDTH;
	font_rect.h = FONT_CHAR_HEIGHT;

	screen_rect.w = FONT_CHAR_WIDTH * dst.w;
	screen_rect.h = FONT_CHAR_HEIGHT * dst.h;
	screen_rect.x = dst.x;
	screen_rect.y = dst.y;

	{
		Uint8 r,g,b,a;
		SDL_GetRenderDrawColor(ctx->rend, &r, &g, &b, &a);
		SDL_SetTextureColorMod(ctx->tex, r, g, b);
		SDL_SetTextureAlphaMod(ctx->tex, a);
	}

	for(; *text; text++)
	{
		Uint8 pos;
		int ret;

		if(*text >= ' ' && *text <= '~')
			pos = *text - ' ';
		else
			pos = '?' - ' ';

		font_rect.x = (pos % FONT_COLUMNS) * FONT_CHAR_WIDTH;
		font_rect.y = (pos / FONT_COLUMNS) * FONT_CHAR_HEIGHT;

		ret = SDL_RenderCopy(ctx->rend, ctx->tex,
		                     &font_rect, &screen_rect);

		if(ret < 0)
			return ret;

		screen_rect.x += screen_rect.w;
	}

	return 0;
}

void FontExit(font_ctx *ctx)
{
	SDL_DestroyTexture(ctx->tex);
	SDL_free(ctx);
}
