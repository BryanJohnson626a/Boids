#include "Model.h"
#include "Graphics.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <SOIL/stb_image_aug.h>
#include <iostream>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <utility>

namespace PE
{
    TextureList Model::loaded_textures;

    Model::Model(std::vector<PE::Mesh> meshes_in) : meshes(std::move(meshes_in))
    {
        for (auto & mesh : meshes)
            mesh.GenerateBuffers();
    }

    static Vec3 MAX, MIN;

    Model::Model(std::string_view path)
    {
        MAX = Vec3{-INFINITY};
        MIN = Vec3{INFINITY};

        LoadAllModels(path);

        // Move and scale the model to fit in the standard cube.
        Vec3 offset = (MAX + MIN) * 0.5f;
        Vec3 scale = 2.f / (MAX - MIN);
        float max_scale = std::max(scale.x, std::max(scale.y, scale.z));

        Mat4 rotation = glm::rotate(Mat4{1}, glm::pi<float>(), West);
        //rotation = rotation * glm::rotate(Mat4{1}, -glm::pi<float>()/2, North);

        for (Mesh & m : meshes)
        {
            for (Vertex & v : m.vertices)
            {
                v.Position = (v.Position - offset) * max_scale;
                v.Position = rotation * Vec4(v.Position.x, v.Position.y, v.Position.z, 1);
                v.Normal = rotation * Vec4(v.Normal.x, v.Normal.y, v.Normal.z, 0);
            }

            m.GenerateBuffers();
        }

    }

    void Model::LoadAllModels(std::string_view directory)
    {
        std::filesystem::path path(directory);

        if (!std::filesystem::exists(directory))
        {
            std::cout << "ERROR: Could not find file or directory: " << directory << std::endl;
            return;
        }

        if (std::filesystem::is_directory(directory))
        {
            std::cout << "Opening " << directory << std::endl;
            for (const auto & subdirectory : std::filesystem::recursive_directory_iterator(directory))
                LoadAllModels(subdirectory.path().string());
        }
        else
        {
            //std::cout << "Loading " << directory << std::endl;
            LoadModel(path.string());
        }

    }

    void Model::LoadModel(std::string_view path)
    {
        Assimp::Importer importer;
        const aiScene * scene = importer.ReadFile(
                path.data(),
                aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        ProcessNode(scene->mRootNode, scene);
    }

    void Model::ProcessNode(aiNode * node, const aiScene * scene)
    {
        // Process all meshes in this node.
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.emplace_back(ProcessMesh(mesh, scene));
        }
        // Recursively process any child nodes.
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i], scene);
    }

    Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        TextureList textures;

        // Retrieve vertex data.
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            if (vector.x > MAX.x)
                MAX.x = vector.x;
            if (vector.y > MAX.y)
                MAX.y = vector.y;
            if (vector.z > MAX.z)
                MAX.z = vector.z;
            if (vector.x < MIN.x)
                MIN.x = vector.x;
            if (vector.y < MIN.y)
                MIN.y = vector.y;
            if (vector.z < MIN.z)
                MIN.z = vector.z;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = glm::normalize(vector);

            // Not all meshes will have texture coordinates.
            if (mesh->mTextureCoords[0])
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            // Push built vertex onto the list.
            vertices.emplace_back(vertex);
        }

        // Faces are assumed to always be triangles
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial * mat = scene->mMaterials[mesh->mMaterialIndex];

            TextureList diffuseMaps = LoadMaterialTextures(
                    mat, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            TextureList specularMaps = LoadMaterialTextures(
                    mat, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        return Mesh(vertices, indices, textures);
    }

    TextureList Model::LoadMaterialTextures(aiMaterial * material, aiTextureType type,
                                            std::string_view type_name)
    {
        TextureList textures;
        for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
        {
            aiString str;
            material->GetTexture(type, i, &str);
            std::string path(str.C_Str());

            textures.emplace_back(GetTexture(path, type));
        }
        return textures;
    }

    uint Model::TextureFromFile(std::string_view path, bool gamma)
    {
        std::string filename = path.data();

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char * data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format = 0;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    Texture * Model::GetTexture(std::string_view path, aiTextureType type)
    {
        for (auto & loaded_texture : loaded_textures)
            if (loaded_texture->path == path)
                return loaded_texture;

        auto * texture = new Texture();
        texture->id = TextureFromFile(path, false);
        texture->type = std::to_string(type);
        texture->path = std::string(path);
        loaded_textures.emplace_back(texture);

        return texture;
    }

    void Model::DrawDebug(Shader * shader, const Mat4 & projection, const Color * color)
    {
        // Load in uniforms.
        Mat4 transform_final = projection * GetTransform();
        glUniformMatrix4fv(shader->uTransform, 1, GL_FALSE, glm::value_ptr(transform_final));

        for (const Mesh & mesh : meshes)
            mesh.DrawColor(shader, color);

        Graphics::LogError(__FILE__, __LINE__);
    }

    void Model::DrawDebugLines(Shader * shader, const Mat4 & projection, const Color * color)
    {
        // Load in uniforms.
        Mat4 transform_final = projection * GetTransform();
        glUniformMatrix4fv(shader->uTransform, 1, GL_FALSE, glm::value_ptr(transform_final));

        for (const Mesh & mesh : meshes)
            mesh.DrawLines(shader, color);

        Graphics::LogError(__FILE__, __LINE__);
    }

    void Model::DrawDeferred(const Shader * shader, const Mat4 & projection, const Vec3 & cam_position)
    {
        // Load in uniforms.
        Mat4 transform_final = projection * GetTransform();
        glUniformMatrix4fv(shader->uTransform, 1, GL_FALSE, glm::value_ptr(transform_final));
        glUniformMatrix4fv(shader->uModelTransform, 1, GL_FALSE, glm::value_ptr(GetTransform()));
        Graphics::LogError(__FILE__, __LINE__);

        for (const Mesh & mesh : meshes)
            mesh.Draw(shader);

        Graphics::LogError(__FILE__, __LINE__);
    }

    void Model::SetMaterial(Material new_material)
    {
        for (auto & mesh : meshes)
            mesh.SetMaterial(new_material);
    }

    void Model::SetRandomMaterials()
    {
        for (auto & mesh : meshes)
            mesh.SetRandomMaterial();
    }

    const std::vector<Mesh> & Model::GetMeshes() const
    {
        return meshes;
    }

    Model::~Model()
    = default;
}
