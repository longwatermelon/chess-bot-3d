#include "rotate.h"

glm::vec3 rotate::point(glm::vec3 p, glm::vec3 angle, glm::vec3 center)
{
    glm::mat3 rotx = {
        1.f, 0.f, 0.f,
        0.f, std::cos(angle.x), -std::sin(angle.x),
        0.f, std::sin(angle.x), std::cos(angle.x)
    };

    glm::mat3 roty = {
        std::cos(angle.y), 0.f, -std::sin(angle.y),
        0.f, 1.f, 0.f,
        std::sin(angle.y), 0.f, std::cos(angle.y)
    };

    glm::mat3 rotz = {
        std::cos(angle.z), -std::sin(angle.z), 0.f,
        std::sin(angle.z), std::cos(angle.z), 0.f,
        0.f, 0.f, 1.f
    };

    return center + (rotz * (rotx * (roty * (p - center))));
}

