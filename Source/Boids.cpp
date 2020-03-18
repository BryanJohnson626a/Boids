#define GLM_ENABLE_EXPERIMENTAL

#include <utility>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "Boids.h"
#include "Engine/Dice.h"
#include "Engine/Graphics.h"

BoidController::BoidController(std::string_view path) : Model(path)
{}

void BoidController::AddBoids(uint num)
{
    Boids.reserve(Boids.size() + num);
    for (uint i = 0; i < num; ++i)
        Boids.emplace_back(MakeBoid());

    updates_per_frame = Boids.size() / 2;
    populates_per_frame = Boids.size() / 120;
}

void BoidController::RemoveBoids(uint num)
{
    int newsize = (int) Boids.size() - (int) num;
    if (newsize > 0)
        Boids.resize(newsize);
    else
        Boids.clear();
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

    for (auto & b : Boids)
        MoveBoid(b, dt);
}

void BoidController::PopulateNeighbors(Boid & boid)
{
    boid.neighbors.clear();

    for (int i = 0; i < Boids.size(); ++i)
    {
        float distance_squared = glm::distance2(boid.position, Boids[i].position);
        if (distance_squared < check_dist_squared && distance_squared != 0)
            boid.neighbors.emplace_back(i);
    }
}

void BoidController::UpdateForce(Boid & boid)
{
    PE::Vec3 avoid_force{}, align_force{}, cohesion_force{}, wander_force{};
    if (!boid.neighbors.empty())
    {
        // Get forces from behaviors.
        boid.force += avoid_force = AvoidVector(boid) * AvoidFactor;
        boid.force += align_force = AlignVector(boid) * AlignFactor;
        boid.force += cohesion_force = CohesionVector(boid) * CohesionFactor;
    }
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

    PE::Mat4 rotation = glm::transpose(glm::lookAt(PE::Vec3{}, boid.velocity, glm::normalize(boid.position)));

    // Update boid transform to new position and heading.
    boid.transform = glm::translate(PE::Mat4(1.f), boid.position) *
                     rotation *
                     PE::Mat4(1.f);
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
    return NewBoid;
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

PE::Vector BoidController::AreaVector(const Boid & boid) const
{
    // Gently nudge boids in if they get too far
    return -boid.position * std::max((glm::length(boid.position) - AreaSize), 0.0f);
}

//----------------------------------------------------------------------------------------------------------------------

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
            glUniformMatrix4fv(shader->uTransform, 1, GL_FALSE, glm::value_ptr(transform_final * boid.transform));
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
            glUniformMatrix4fv(shader->uTransform, 1, GL_FALSE, glm::value_ptr(boid.transform * transform_final));
            glDrawElements(GL_LINE_STRIP, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);
        }
        glBindVertexArray(0);
    }
    PE::Graphics::LogError(__FILE__, __LINE__);
}

void BoidController::DrawDeferred(const PE::Shader * shader, const PE::Mat4 & projection, const PE::Vec3 & cam_position)
{
    // Draw each boid using it's individual position and rotation.
    PE::Mat4 transform_final = projection * GetTransform();
    auto model_transform = GetTransform();

    // Draw one mesh at a time to reduce uniform setting.
    for (const PE::Mesh & mesh : meshes)
    {
        // Load in mesh data.
        glUniform3fv(shader->uObjectColor, 1, &mesh.material.ambient.R);
        glUniform3fv(shader->uDiffuse, 1, &mesh.material.diffuse.R);
        glUniform3fv(shader->uSpecular, 1, &mesh.material.specular.R);
        glUniform1f(shader->uShininess, mesh.material.shininess);
        PE::Graphics::LogError(__FILE__, __LINE__);
        glBindVertexArray(mesh.VAO);

        // Draw each boid.
        for (const Boid & boid : Boids)
        {
            glUniformMatrix4fv(shader->uTransform, 1, GL_FALSE,
                               glm::value_ptr(transform_final * boid.transform * glm::scale(BoidScale)));
            glUniformMatrix4fv(shader->uModelTransform, 1, GL_FALSE,
                               glm::value_ptr(model_transform * boid.transform * glm::scale(BoidScale)));
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
        }

        glBindVertexArray(0);
    }

    PE::Graphics::LogError(__FILE__, __LINE__);
}
