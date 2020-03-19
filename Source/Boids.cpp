#define GLM_ENABLE_EXPERIMENTAL

#include <utility>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "Boids.h"
#include "Engine/Dice.h"
#include "Engine/Graphics.h"

BoidController::BoidController(std::string_view path) : Model(path)
{
    // vertex buffer object
    glGenBuffers(1, &BoidDataBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, BoidDataBuffer);

    // Add instance transform attributes to mesh VAOs.
    for (auto & mesh : meshes)
    {
        unsigned int VAO = mesh.VAO;
        glBindVertexArray(VAO);
        // vertex attributes
        std::size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) 0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (1 * vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BoidController::AddBoids(uint num)
{
    Boids.reserve(Boids.size() + num);
    for (uint i = 0; i < num; ++i)
        Boids.emplace_back(MakeBoid());

    BoidData.resize(Boids.size());

    updates_per_frame = Boids.size() / 2;
    populates_per_frame = Boids.size() / 120;
}

void BoidController::RemoveBoids(uint num)
{
    int newsize = (int) Boids.size() - (int) num;
    if (newsize > 0)
    {
        Boids.resize(newsize);
        BoidData.resize(newsize);
    }
    else
    {
        Boids.clear();
        BoidData.clear();
    }
}

void BoidController::Update(float dt)
{
    if (Boids.empty())
        return;

    for (int i = 0; i < populates_per_frame; ++i)
    {
        populates_counter = (populates_counter + 1) % Boids.size();
        PopulateNeighbors(Boids[populates_counter]);
    }

    for (int i = 0; i < updates_per_frame; ++i)
    {
        updates_counter = (updates_counter + 1) % Boids.size();
        UpdateForce(Boids[updates_counter]);
    }

    for (int i = 0; i < Boids.size(); ++i)
    {
        MoveBoid(Boids[i], dt);
        UpdateTransform(Boids[i], BoidData[i]);
    }
}

void BoidController::PopulateNeighbors(Boid & boid)
{
    boid.neighbors.clear();

    for (int i = 0; i < Boids.size(); ++i)
    {
        float distance_squared = glm::distance2(boid.position, Boids[i].position);
        if (distance_squared < neighbor_dist_squared && distance_squared != 0)
            boid.neighbors.emplace_back(i);
    }

    for (uint feared_group = 0; feared_group < FearedBoids.size(); ++feared_group)
    {
        boid.fear_neighbors[feared_group].clear();
        for (uint feared_boid = 0; feared_boid < FearedBoids[feared_group]->Boids.size(); ++feared_boid)
        {
            float distance_squared = glm::distance2(boid.position,
                                                    FearedBoids[feared_group]->Boids[feared_boid].position);
            if (distance_squared < fear_dist_squared && distance_squared != 0)
                boid.fear_neighbors[feared_group].emplace_back(feared_boid);
        }
    }
}

void BoidController::UpdateForce(Boid & boid)
{
    PE::Vec3 avoid_force{}, align_force{}, cohesion_force{}, fear_force{};
    if (!boid.neighbors.empty())
    {
        // Get forces from behaviors.
        boid.force += avoid_force = AvoidVector(boid) * AvoidFactor;
        boid.force += align_force = AlignVector(boid) * AlignFactor;
        boid.force += cohesion_force = CohesionVector(boid) * CohesionFactor;
    }
    boid.force += fear_force = FearVector(boid) * FearFactor;
    PE::Vec3 area_force = AreaVector(boid) * AreaFactor;
    boid.force += area_force;

    // Can't normalize a 0 vector and velocity isn't changing so just stop.
    if (boid.force == PE::Vec3{0})
        return;

    boid.force = glm::normalize(boid.force);
}

void BoidController::MoveBoid(Boid & boid, float dt)
{
    // Add the force to shift the direction of the velocity toward where the boid
    // wants to go, then scale that velocity to move speed.
    boid.velocity += boid.force * turnForce * dt;
    boid.velocity = glm::normalize(boid.velocity) * boid.speed;

    // Update position.
    boid.position += boid.velocity * dt;
}

void BoidController::UpdateTransform(const BoidController::Boid & boid, PE::Mat4 & boid_render_info)
{
    // Update boid transform to new position and heading.
    // Using boid position as "Up" vector means up is always away from the center.
    boid_render_info = glm::translate(boid.position) *
                       glm::transpose(glm::lookAt(PE::Vec3{}, boid.velocity, boid.position)) *
                       glm::scale(BoidScale);
}

PE::Vector RandomVec()
{
    static DieReal VelDie(-1.f, 1.f);
    return PE::Vector{VelDie.Roll(), VelDie.Roll(), VelDie.Roll()};
}

BoidController::Boid BoidController::MakeBoid()
{
    // Random generators for use in this function only.
    static DieReal PosDie(-30, 30);
    static DieReal SpeedDie(1, 2);

    // Create boid with random location and velocity.
    Boid NewBoid;
    NewBoid.position = PE::Vector{PosDie.Roll(), PosDie.Roll(), PosDie.Roll()};
    NewBoid.velocity = RandomVec();
    NewBoid.velocity = glm::normalize(NewBoid.velocity);
    NewBoid.speed = SpeedDie.Roll();
    NewBoid.fear_neighbors.resize(FearedBoids.size());
    return NewBoid;
}

void BoidController::AddFearedBoids(const BoidController * feared_boids)
{
    // Avoid adding the same boids twice.
    for (auto * b : FearedBoids)
        if (b == feared_boids)
            return;

    FearedBoids.emplace_back(feared_boids);
    // Since we added a new group of feared boids, we need to add a spot for it to each boid's list.
    for (auto & b : Boids)
        b.fear_neighbors.emplace_back(std::vector<uint>());
}

void BoidController::RemoveFearedBoids(const BoidController * removed_fear)
{
    int remove_index = -1;
    for (uint i = 0; i < FearedBoids.size(); ++i)
        if (FearedBoids[i] == removed_fear)
        {
            remove_index = i;
            break;
        }

    // Cant remove something not on the list.
    if (remove_index == -1)
        return;

    // Remove feared boids from controller and each boids' neighbor list.
    FearedBoids.erase(FearedBoids.begin() + remove_index);
    for (auto & b : Boids)
        b.fear_neighbors.erase(b.fear_neighbors.begin() + remove_index);
}

void BoidController::SetNeighborDistance(float distance)
{
    neighbor_dist_squared = distance * distance;
}

void BoidController::SetFearDistance(float distance)
{
    fear_dist_squared = distance * distance;
}

//----------------------------------------------------------------------------------------------------------------------
// Behavior code.

PE::Vector BoidController::AvoidVector(const Boid & boid) const
{
    PE::Vector bVector{};

    for (auto i : boid.neighbors)
    {
        float dist = glm::distance2(Boids[i].position, boid.position);
        if (dist != 0.f)
            bVector += (boid.position - Boids[i].position) / dist;
    }
    return bVector;
}

PE::Vector BoidController::AlignVector(const Boid & boid) const
{
    PE::Vector bVector{};

    for (auto i : boid.neighbors)
        bVector += Boids[i].velocity;

    bVector = glm::normalize(bVector);

    return bVector;
}

PE::Vector BoidController::CohesionVector(const Boid & boid) const
{
    PE::Vector bVector{};

    for (auto i : boid.neighbors)
        bVector += Boids[i].position - boid.position;

    bVector = glm::normalize(bVector);
    return bVector;
}

PE::Vector BoidController::FearVector(const BoidController::Boid & boid) const
{
    PE::Vector bVector{};

    // For each group of feared boids, calculate for each neighboring boid from that group.
    for (uint feared_group = 0; feared_group < boid.fear_neighbors.size(); ++feared_group)
        for (uint feared_boid = 0; feared_boid < boid.fear_neighbors[feared_group].size(); ++feared_boid)
            bVector += boid.position - FearedBoids[feared_group]->Boids[feared_boid].position;

    if (bVector != PE::Vec3{0})
        bVector = glm::normalize(bVector);

    return bVector;
}

PE::Vector BoidController::AreaVector(const Boid & boid) const
{
    // Gently nudge boids in if they get too far
    return -boid.position * std::max((glm::length(boid.position) - AreaSize), 0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
// Render code.

void BoidController::DrawDebug(PE::Shader * shader, const PE::Mat4 & projection, const PE::Color * color)
{
    // Draw each boid using it's individual position and rotation.
    PE::Mat4 transform_final = projection * GetTransform();

    // Draw one mesh at a time to reduce uniform setting.
    for (const PE::Mesh & mesh : meshes)
    {
        glUniform4fv(shader->uObjectColor, 1, &color->R);
        glBindVertexArray(mesh.VAO);
        PE::Graphics::LogError(__FILE__, __LINE__);

        // Draw each boid using it's individual position and rotation.
        for (const Boid & boid : Boids)
        {
            glUniformMatrix4fv(shader->uTransform, 1, GL_FALSE, glm::value_ptr(transform_final));
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
        }
        glBindVertexArray(0);
    }
    PE::Graphics::LogError(__FILE__, __LINE__);
}

void BoidController::DrawDebugLines(PE::Shader * shader, const PE::Mat4 & projection, const PE::Color * color)
{
    // Draw each boid using it's individual position and rotation.
    PE::Mat4 transform_final = projection * GetTransform();

    // Draw one mesh at a time to reduce uniform setting.
    for (const PE::Mesh & mesh : meshes)
    {
        glUniform4fv(shader->uObjectColor, 1, &color->R);
        glBindVertexArray(mesh.VAO);
        PE::Graphics::LogError(__FILE__, __LINE__);

        // Draw each boid.
        for (const Boid & boid : Boids)
        {
            glUniformMatrix4fv(shader->uTransform, 1, GL_FALSE, glm::value_ptr(transform_final));
            glDrawElements(GL_LINE_STRIP, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
        }
        glBindVertexArray(0);
    }
    PE::Graphics::LogError(__FILE__, __LINE__);
}

void BoidController::DrawDeferred(const PE::Shader * shader, const PE::Mat4 & projection, const PE::Vec3 & cam_position)
{
    PE::Mat4 transform = projection * GetTransform();

    glBindBuffer(GL_ARRAY_BUFFER, BoidDataBuffer);
    glBufferData(GL_ARRAY_BUFFER, Boids.size() * sizeof(glm::mat4), &BoidData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Draw one mesh at a time to reduce uniform setting.
    for (const PE::Mesh & mesh : meshes)
    {
        // Load in mesh data.
        glUniform3fv(shader->uObjectColor, 1, &mesh.material.ambient.R);
        glUniform3fv(shader->uDiffuse, 1, &mesh.material.diffuse.R);
        glUniform3fv(shader->uSpecular, 1, &mesh.material.specular.R);
        glUniform1f(shader->uShininess, mesh.material.shininess);
        glUniformMatrix4fv(shader->uTransform, 1, GL_FALSE, glm::value_ptr(transform));
        glBindVertexArray(mesh.VAO);
        PE::Graphics::LogError(__FILE__, __LINE__);

        // Draw all boids at once.
        glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr, Boids.size());
        PE::Graphics::LogError(__FILE__, __LINE__);

        glBindVertexArray(0);
    }

    PE::Graphics::LogError(__FILE__, __LINE__);
}
