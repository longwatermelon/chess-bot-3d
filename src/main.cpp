#include "prog.h"
#include "render.h"

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_Window *w = SDL_CreateWindow("Chess 3D",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCRSIZE, SCRSIZE, SDL_WINDOW_SHOWN);
    SDL_Renderer *r = SDL_CreateRenderer(w, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    {
        Prog p(w, r);
        p.mainloop();
    }

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

