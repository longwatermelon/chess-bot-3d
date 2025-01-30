#include "render.h"
#include <SDL2/SDL.h>

static glm::vec2 project(glm::vec3 p)
{
    return glm::vec2(
        ((p.x / p.z) + .5f) * SCRSIZE,
        ((p.y / p.z) + .5f) * SCRSIZE
    );
}

struct SCInfo
{
    float x, z, sx, sz;
};

static void scanline(int ty, int by, SCInfo &left, SCInfo &right, const Tri &tri, uint32_t *scr, float *zbuf)
{
    for (int y = ty; y < by; ++y)
    {
        float z = left.z;
        float sz = (right.z - left.z) / (right.x - left.x);

        for (int x = left.x; x <= right.x; ++x)
        {
            z += sz;

            if (x < 0)
            {
                z += sz * -x;
                x = -1;
                continue;
            }

            if (x >= SCRSIZE) break;

            int i = y * SCRSIZE + x;
            if (i >= 0 && i < SCRSIZE * SCRSIZE)
            {
                if (z < zbuf[i])
                {
                    zbuf[i] = z;

                    glm::vec3 col = glm::vec3(tri.color.r, tri.color.g, tri.color.b);
                    glm::vec3 shaded = (std::max(0.f, 1.f - glm::dot(tri.norm, glm::vec3(0.f, 0.f, 1.f)))) * col;
                    scr[i] = 0x00000000 | (int)(shaded.x) << 16 | (int)(shaded.y) << 8 | (int)(shaded.z);
                }
            }
        }

        left.x += left.sx;
        right.x += right.sx;

        left.z += left.sz;
        right.z += right.sz;
    }
}

void rend::triangle(Tri t, uint32_t *scr, float *zbuf)
{
    if (t.verts[0].z <= .5f || t.verts[1].z <= .5f || t.verts[2].z <= .5f)
        return;

    if (t.norm.z > 0.f)
        return;

    std::array<glm::vec3, 3> proj;
    for (int i = 0; i < 3; ++i)
    {
        glm::vec2 p = project(t.verts[i]);
        proj[i] = glm::vec3(p.x, p.y, t.verts[i].z);
    }

    if (proj[0].y > proj[1].y) std::swap(proj[0], proj[1]);
    if (proj[0].y > proj[2].y) std::swap(proj[0], proj[2]);
    if (proj[1].y > proj[2].y) std::swap(proj[1], proj[2]);

    auto gen_sc = [proj](int a, int b){
        float dy = proj[b].y - proj[a].y;
        return SCInfo{
            .x = std::round(proj[a].x), .z = proj[a].z,
            .sx = (proj[b].x - proj[a].x) / dy,
            .sz = (proj[b].z - proj[a].z) / dy
        };
    };

    SCInfo s02 = gen_sc(0, 2),
           s01 = gen_sc(0, 1),
           s12 = gen_sc(1, 2);

    scanline(proj[0].y, proj[1].y, s02.sx < s01.sx ? s02 : s01, s02.sx > s01.sx ? s02 : s01, t, scr, zbuf);
    scanline(proj[1].y, proj[2].y, s02.x < s12.x ? s02 : s12, s02.x > s12.x ? s02 : s12, t, scr, zbuf);
}

