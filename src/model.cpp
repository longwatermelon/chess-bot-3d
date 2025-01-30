#include "model.h"
#include "rotate.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <glm/gtx/intersect.hpp>

static std::vector<std::string> split_string(std::string str, const std::string &delim)
{
    std::vector<std::string> res;
    size_t pos = 0;
    std::string tok;

    while ((pos = str.find(delim)) != std::string::npos)
    {
        res.emplace_back(str.substr(0, pos));
        str.erase(0, pos + delim.size());
    }

    res.emplace_back(str);
    return res;
}

Model::Model(const std::string &src)
{
    std::ifstream ifs(src);
    std::string buf;
    std::vector<std::string> lines;

    while (std::getline(ifs, buf))
        lines.emplace_back(buf);

    size_t n = 0;
    while (lines[n][0] != 'v') ++n;

    std::vector<glm::vec3> verts;
    std::vector<glm::vec3> norms;

    while (lines[n].substr(0, 2) == "v ")
    {
        glm::vec3 v;
        char c;
        std::stringstream(lines[n]) >> c >> v.x >> v.y >> v.z;
        verts.emplace_back(v);
        ++n;
    }

    while (lines[n].substr(0, 2) == "vn")
    {
        glm::vec3 norm;
        std::string s;
        std::stringstream(lines[n]) >> s >> norm.x >> norm.y >> norm.z;
        norms.emplace_back(norm);
        ++n;
    }

    while (lines[n][0] != 'f') ++n;

    while (n < lines.size() && lines[n][0] == 'f')
    {
        Tri t;
        std::vector<std::string> split = split_string(lines[n], " ");

        for (int i = 0; i < 3; ++i)
        {
            std::vector<std::string> s = split_string(split[i + 1], "/");
            t.verts[i] = verts[std::stoi(s[0]) - 1];
            t.norm = norms[std::stoi(s[2]) - 1];
        }

        t.color = { 255, 255, 255 };
        m_tris.emplace_back(t);
        ++n;
    }

    /* for (auto &t : m_tris) */
    /* { */
    /*     std::cout << "Verts: "; */
    /*     for (auto &v : t.verts) */
    /*         std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ") "; */
    /*     std::cout << " | Norm: " << t.norm.x << ", " << t.norm.y << ", " << t.norm.z << "\n"; */
    /* } */
}

Model::~Model()
{
}

static Tri transform_tri(glm::vec3 mpos, glm::vec3 mrot, glm::vec3 center, Tri t)
{
    for (auto &v : t.verts)
    {
        v += mpos;
        v = rotate::point(v, mrot, center);
    }

    t.norm = rotate::point(t.norm, mrot);
    return t;
}

void Model::render(glm::vec3 pos, glm::vec3 rot, glm::vec3 center, SDL_Color col, uint32_t *scr, float *zbuf)
{
    for (auto &t : m_tris)
    {
        Tri tmp = transform_tri(pos, rot, center, t);
        tmp.color = col;
        rend::triangle(tmp, scr, zbuf);
    }
}

bool Model::ray_intersect(glm::vec3 ray, glm::vec3 pos, glm::vec3 rot, glm::vec3 center, float *nearest)
{
    *nearest = INFINITY;

    for (auto &t : m_tris)
    {
        Tri tri = transform_tri(pos, rot, center, t);

        glm::vec2 bary;
        float dist;
        if (glm::intersectRayTriangle(glm::vec3(0.f), ray,
            tri.verts[0], tri.verts[1], tri.verts[2], bary, dist) && dist < *nearest)
            *nearest = dist;
    }

    return *nearest < INFINITY;
}

