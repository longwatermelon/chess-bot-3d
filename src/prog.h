#pragma once
#include "render.h"
#include "board.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Prog
{
public:
    Prog(SDL_Window *w, SDL_Renderer *r);
    ~Prog();

    void mainloop();

private:
    void reset_buffers();

private:
    bool m_running{ true };
    SDL_Window *m_window{ nullptr };
    SDL_Renderer *m_rend{ nullptr };
    TTF_Font *m_font{ nullptr };

    uint32_t m_scr[600 * 600];
    float m_zbuf[600 * 600];
    SDL_Texture *m_scrtex{ nullptr };

    Board m_board;
    bool m_rmb{ false };
};

