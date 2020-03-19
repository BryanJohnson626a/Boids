#pragma once

#include <vector>
#include <array>
#include "Engine/Types.h"
#include "Engine/Transformable.h"
#include "Engine/Model.h"

const uint GRID_SIZE = 256;

class BoidController : public PE::Model
{
    struct GridPos
    {
        uint x, y, z;
        bool operator==(const GridPos & other);
        bool operator!=(const GridPos & other);
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
        GridPos grid_position{0, 0, 0};
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
    PE::Vec3 BoidScale{1};

    // How many boids should recalculate their heading each frame.
    uint updates_per_frame = 1000;

    // How many boids should recalculate their grid position each frame.
    uint grid_updates_per_frame = 1000;

    // How mant boids should repopulate their neighbor list each frame.
    uint populates_per_frame = 1000;

    void AddFearedBoids(const BoidController * feared_boids);
    void RemoveFearedBoids(const BoidController * removed_fear);
    void SetFearDistance(float distance);
    void SetNeighborDistance(float distance);
    void SetAreaSize(float size);
private:
    Boid MakeBoid();
    void PopulateGrid();
    void PopulateNeighbors(Boid & boid);
    void UpdateForce(Boid & boid);
    void MoveBoid(Boid & boid, float dt);
    void UpdateGridPosition(uint boid_index);
    void UpdateTransform(const Boid & boid, PE::Mat4 & boid_render_info);

    [[nodiscard]] PE::Vector AvoidVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector AlignVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector CohesionVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector FearVector(const Boid & boid) const;
    [[nodiscard]] PE::Vector AreaVector(const Boid & boid) const;

    GridPos GetGridPosition(const PE::Vec3 & position);
    void UpdateNeighborSearchDistance();

    // The size of area boids try to stay within.
    float AreaSize = 10;
    float grid_offset = 0;
    float grid_size = 0;
    float neighbor_dist_squared = 1;
    int neighbor_search_distance = 1;
    float fear_dist_squared = 25;

    std::vector<const BoidController *> FearedBoids;
    float turnForce = 1;
    std::vector<Boid> Boids;
    std::vector<PE::Mat4> BoidData;

    // 3d array that represents position in space, with each vector
    // containing the IDs of the boids within that section.
    std::array<std::array<std::array<std::vector<uint>,
            GRID_SIZE>, GRID_SIZE>, GRID_SIZE> PositionGrid;

    GLuint BoidDataBuffer = 0;

    uint populates_counter = 0;
    uint updates_counter = 0;
    uint grid_updates_counter = 0;
};
