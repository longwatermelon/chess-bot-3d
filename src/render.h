#pragma once
#include <array>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#define SCRSIZE 600.f

struct Tri
{
    std::array<glm::vec3, 3> verts;
    SDL_Color color;
    glm::vec3 norm;
};

namespace rend
{
    void triangle(Tri t, uint32_t *scr, float *zbuf);
}

