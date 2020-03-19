// All content � 2018 DigiPen (USA) Corporation, all rights reserved.
/*!
@filename Mesh.cpp
@author   Bryan Johnson
*/

#include <algorithm>
#include <fstream>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <utility>

#include "Mesh.h"
#include "Graphics.h"

namespace PE
{

    Mesh::Mesh(std::vector<Vertex> vertices,
               std::vector<uint> indices,
               TextureList textures) :
            vertices(std::move(vertices)),
            indices(std::move(indices)),
            textures(std::move(textures))
    {
    }

    // Generates GPU buffers.
    void Mesh::GenerateBuffers()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint),
                     &indices[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) nullptr);

        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));

        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
        Graphics::LogError(__FILE__, __LINE__);

    }

    // Delete GPU buffers.
    void Mesh::ClearBuffers()
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
        glDeleteBuffers(1, &EBO);
        EBO = 0;
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    Mesh::~Mesh()
    {
        ClearBuffers();
    }

    void Mesh::Draw(const Shader * shader) const
    {
        glUniform3fv(shader->uObjectColor, 1, &material.ambient.R);
        glUniform3fv(shader->uDiffuse, 1, &material.diffuse.R);
        glUniform3fv(shader->uSpecular, 1, &material.specular.R);
        glUniform1f(shader->uShininess, material.shininess);
        Graphics::LogError(__FILE__, __LINE__);

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Mesh::DrawColor(const Shader * shader, const Color * color) const
    {
        glUniform4fv(shader->uObjectColor, 1, &color->R);
        Graphics::LogError(__FILE__, __LINE__);

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Mesh::DrawLines(const Shader * shader, const Color * color) const
    {
        glUniform4fv(shader->uObjectColor, 1, &color->R);
        Graphics::LogError(__FILE__, __LINE__);

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_LINE_STRIP, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Mesh::SetRandomMaterial()
    {
        material.ambient.R = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        material.ambient.G = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        material.ambient.B = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        material.diffuse.R = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        material.diffuse.G = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        material.diffuse.B = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        material.specular.R = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        material.specular.G = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        material.specular.B = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        material.shininess = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 128;
    }

    void Mesh::SetMaterial(Material new_material)
    {
        material = new_material;
    }
}
