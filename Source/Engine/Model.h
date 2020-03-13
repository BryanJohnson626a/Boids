#pragma once

#include <assimp/scene.h>
#include "Types.h"
#include "Color.h"
#include "Mesh.h"
#include "Transformable.h"

namespace PE
{
    /*!
    @brief The base class for everything that can be rendered to the screen.
       A model is also a transformable, allowing it to be moved, scaled,
       and rotated in world-space.
    */
    class Model : public Transformable
    {
        static Texture * GetTexture(std::string_view path, aiTextureType type);
        static TextureList LoadMaterialTextures(
                aiMaterial * material, aiTextureType type,
                std::string_view type_name);

        static uint TextureFromFile(std::string_view path, bool gamma);

        static TextureList loaded_textures;
    public:
        // Use to create a single-mesh model.
        explicit Model(const Mesh & mesh);
        explicit Model(std::string_view path);
        ~Model() override;

        void DrawDebug(Shader * shader, const Mat4 & projection,
                       const Color * color);
        void DrawDebugLines(Shader * shader, const Mat4 & projection,
                            const Color * color);
        void DrawDeferred(const Shader * shader, const Mat4 & projection,
                          const Vec3 & cam_position);
        void SetMaterial(Material new_material);
        void SetRandomMaterials();

        [[nodiscard]] const std::vector<Mesh> & GetMeshes() const;
    protected:
        void LoadAllModels(std::string_view directory);
        void LoadModel(std::string_view path);
        void ProcessNode(aiNode * node, const aiScene * scene);
        static Mesh ProcessMesh(aiMesh * mesh, const aiScene * scene);

        std::vector<Mesh> meshes;
    };
}
