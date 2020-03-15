#pragma once

#include <vector>
#include <array>
#include "Engine/Types.h"
#include "Engine/Transformable.h"
#include "Engine/Model.h"

const PE::Vec3 BOUNDS{30.f, 30.f, 30.f};

class BoidController : public PE::Model
{
    struct Boid
    {
        PE::Vector position;
        PE::Vector velocity{};
        PE::Vector new_velocity{};
        std::vector<const Boid *> neighbors;
        glm::mat4 transform;
    };

public:
    explicit BoidController(std::vector<PE::Mesh> meshes);
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
    PE::Vec3 BoidScale{1};

    // How mant boids should repopulate their neighbor list each frame.
    uint populates_per_frame = 500;

    // How mant boids should recalculate their heading each frame.
    uint updates_per_frame = 5000;
private:
    static Boid MakeBoid();
    void PopulateNeighbors(Boid & boid);
    void UpdateBoid(Boid & boid, float dt);
    static void MoveBoid(Boid & boid, float dt);

    [[nodiscard]] PE::Vector AvoidVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector AlignVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector CohesionVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector AreaVector(const Boid & boid) const;

    [[nodiscard]] bool InRange(const Boid & A, const Boid & B);

    const BoidController * FearedBoids;
    PE::Vector AreaMax{BOUNDS};
    PE::Vector AreaMin{-BOUNDS};
    float speed = 1;
    float turnForce = 2;
    float check_dist_squared = 5;
    std::vector<Boid> Boids;

    uint populates_counter = 0;
    uint updates_counter = 0;
};
