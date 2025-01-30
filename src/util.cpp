#include "util.h"

SDL_Texture *util::render_text(SDL_Renderer *rend, TTF_Font *font, const char *text)
{
    if (text[0] == '\0')
        return nullptr;

    SDL_Surface *surf = TTF_RenderText_Blended(font, text, { 255, 255, 255 });
    SDL_Texture *tex = SDL_CreateTextureFromSurface(rend, surf);

    SDL_FreeSurface(surf);
    return tex;
}

