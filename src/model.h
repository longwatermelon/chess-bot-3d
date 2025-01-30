#pragma once
#include "render.h"
#include <string>
#include <vector>

class Model
{
public:
    Model(const std::string &src);
    ~Model();

    void render(glm::vec3 pos, glm::vec3 rot, glm::vec3 center, SDL_Color col, uint32_t *scr, float *zbuf);
    bool ray_intersect(glm::vec3 ray, glm::vec3 pos, glm::vec3 rot, glm::vec3 center, float *nearest);

    void move(glm::vec3 dir);
    void rotate(glm::vec3 rot);

private:
    std::vector<Tri> m_tris;
};

