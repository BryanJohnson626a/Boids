#pragma once

#include <glm/glm.hpp>
#include <vector>

typedef unsigned int uint;

namespace PE
{
    using Mat4 = glm::mat4x4;
    using Mat3 = glm::mat3x3;
    using Vec2 = glm::vec2;
    using Vec3 = glm::vec3;
    using Vector = glm::vec3;
    using Point = glm::vec3;
    using Vec4 = glm::vec4;
    using Radian = float;

    const Vec3 Up{0, 0, 1};
    const Vec3 Down{0, 0, -1};
    const Vec3 North{1, 0, 0};
    const Vec3 East{0, 1, 0};
    const Vec3 South{-1, 0, 0};
    const Vec3 West{0, -1, 0};

    class Texture;
    class Shader;
    class Model;
    class Mesh;

    using TextureList = std::vector<Texture *>;
}
