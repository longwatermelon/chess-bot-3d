#pragma once
#include <glm/glm.hpp>

namespace rotate
{
    // YXZ rotation
    glm::vec3 point(glm::vec3 p, glm::vec3 angle, glm::vec3 center = glm::vec3(0.f));
}

