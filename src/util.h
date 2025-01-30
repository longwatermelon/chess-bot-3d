#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace util
{
    SDL_Texture *render_text(SDL_Renderer *rend, TTF_Font *font, const char *text);
}

