/*!
@filename Mesh.h
@author   Bryan Johnson
*/

#pragma once

#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <map>
#include <string>

#include "Types.h"
#include "Color.h"

namespace PE
{
    struct Vertex
    {
        Vec3 Position{};
        Vec3 Normal{};
        Vec2 TexCoords{};
    };

    struct Texture
    {
        uint id;
        std::string type;
        std::string path;
    };

    class Mesh
    {
    public:
        Mesh(std::vector<Vertex> vertices,
             std::vector<uint> indices,
             TextureList textures);
        ~Mesh();

        void Draw(const Shader * shader) const;
        void DrawColor(const Shader * shader, const Color * color) const;
        void DrawLines(const Shader * shader, const Color * color) const;

        void SetMaterial(Material new_material);
        void SetRandomMaterial();

        std::vector<Vertex> vertices;
        std::vector<uint> indices;
        TextureList textures;
        void GenerateBuffers();
    protected:
        void ClearBuffers();

        Material material;

        GLuint VAO = 0, VBO = 0, EBO = 0;
    };
}
