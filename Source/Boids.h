#pragma once

#include <vector>
#include <array>
#include "Engine/Types.h"
#include "Engine/Transformable.h"
#include "Engine/Model.h"

class BoidController : public PE::Model
{
    struct BoidRenderInfo
    {
        glm::mat4 transform{};
    };
    struct Boid
    {
        PE::Vector position{};
        PE::Vector velocity{};
        PE::Vector force{};
        // Store neighbors by index to avoid
        // pointer invalidation when adding boids.
        std::vector<uint> neighbors;
        std::vector<std::vector<uint>> fear_neighbors;
        float speed{1};
    };

public:
    explicit BoidController(std::string_view path);

    void AddBoids(uint num);
    void RemoveBoids(uint num);

    void Update(float dt);

    void DrawDebug(PE::Shader * shader, const PE::Mat4 & projection,
                   const PE::Color * color) override;

    void DrawDebugLines(PE::Shader * shader, const PE::Mat4 & projection,
                        const PE::Color * color) override;

    void DrawDeferred(const PE::Shader * shader,
                      const PE::Mat4 & projection,
                      const PE::Vec3 & cam_position) override;

    bool dynamic_color{true};

    // Behavior attributes.
    float AvoidFactor = 1;
    float AlignFactor = 1;
    float CohesionFactor = 1;
    float AreaFactor = 1;
    float FearFactor = 1;
    // The size of area boids try to stay within.
    float AreaSize = 10;
    PE::Vec3 BoidScale{1};

    // How mant boids should repopulate their neighbor list each frame.
    uint populates_per_frame = 500;

    // How mant boids should recalculate their heading each frame.
    uint updates_per_frame = 5000;

    void AddFearedBoids(const BoidController * feared_boids);
    void RemoveFearedBoids(const BoidController * removed_fear);
    void SetNeighborDistance(float distance);
    void SetFearDistance(float distance);
private:
    Boid MakeBoid();
    void PopulateNeighbors(Boid & boid);
    void UpdateForce(Boid & boid);
    void MoveBoid(Boid & boid, float dt);
    void UpdateTransform(const Boid & boid, PE::Mat4& boid_render_info);

    [[nodiscard]] PE::Vector AvoidVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector AlignVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector CohesionVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector FearVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector AreaVector(const Boid & boid) const;

    std::vector<const BoidController *> FearedBoids;
    float turnForce = 1;
    float neighbor_dist_squared = 5;
    float fear_dist_squared = 25;
    std::vector<Boid> Boids;
    //std::vector<BoidRenderInfo> BoidData;
    std::vector<PE::Mat4> BoidData;

    GLuint BoidDataBuffer = 0;

    uint populates_counter = 0;
    uint updates_counter = 0;
};
