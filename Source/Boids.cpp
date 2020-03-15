#define GLM_ENABLE_EXPERIMENTAL

#include <utility>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "Boids.h"
#include "Engine/Dice.h"
#include "Engine/Graphics.h"

BoidController::BoidController(std::vector<PE::Mesh> meshes_in) : Model(std::move(meshes_in))
{}

BoidController::BoidController(std::string_view path) : Model(path)
{}

void BoidController::AddBoids(uint num)
{
    Boids.reserve(Boids.size() + num);
    for (uint i = 0; i < num; ++i)
        Boids.emplace_back(MakeBoid());

    updates_per_frame = Boids.size() / 2;
    populates_per_frame = Boids.size() / 20;
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
        UpdateBoid(Boids[updates_counter], dt);
    }

    for (auto & b : Boids)
        MoveBoid(b, dt);
}

bool BoidController::InRange(const Boid & A, const Boid & B)
{
    float distance_squared = glm::distance2(A.position, B.position);
    return distance_squared < check_dist_squared && distance_squared != 0;
}

void BoidController::PopulateNeighbors(Boid & boid)
{
    boid.neighbors.clear();
    for (const auto & other : Boids)
        if (InRange(boid, other))
            boid.neighbors.emplace_back(&other);
}

void BoidController::MoveBoid(Boid & boid, float dt)
{
    // Switch velocity to the newly calculated velocity, and update position.
    boid.velocity = boid.new_velocity;
    boid.position += boid.velocity * dt;

    PE::Mat4 rotation = glm::transpose(glm::lookAt(PE::Vec3{}, boid.velocity, glm::normalize(boid.position)));
    rotation[3][3] = 1;

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
    static DieReal XDie(-BOUNDS.x * 0.75f, BOUNDS.x * 0.75f);
    static DieReal YDie(-BOUNDS.y * 0.75f, BOUNDS.y * 0.75f);
    static DieReal ZDie(-BOUNDS.z * 0.75f, BOUNDS.z * 0.75f);

    // Create boid with random location and velocity.
    Boid NewBoid;
    NewBoid.position = PE::Vector{XDie.Roll(), YDie.Roll(), ZDie.Roll()};
    NewBoid.velocity = RandomVec();
    NewBoid.velocity = glm::normalize(NewBoid.velocity);
    return NewBoid;
}

void BoidController::UpdateBoid(BoidController::Boid & boid, float dt)
{
    PE::Vector force;
    if (!boid.neighbors.empty())
    {
        // Get forces from behaviors.
        auto avoid_force = AvoidVector(boid) * AvoidFactor;
        auto align_force = AlignVector(boid) * AlignFactor;
        auto cohesion_force = CohesionVector(boid) * CohesionFactor;
        auto area_force = AreaVector(boid) * AreaFactor;

        force = avoid_force + align_force + cohesion_force + area_force;
    }
    else
    {
        // Wander
        force = RandomVec() * 0.1f;
    }



    // If force is zero, no change in velocity.
    if (force == PE::Vector{0})
    {
        boid.new_velocity = boid.velocity;
        return;
    }

    // To maintain consistent turning speed, normalize the force and apply turn factor and DT mod.
    force = glm::normalize(force);
    force *= turnForce * dt;

    // New velocity still has the old velocity in it. We add the force to shift the direction of
    // the velocity toward where the boid wants to go, then scale that velocity to move speed.
    boid.new_velocity += force;
    boid.new_velocity = glm::normalize(boid.new_velocity) * speed;


    // Bounce at edges.
    for (int i = 0; i < 3; ++i)
        if ((boid.position[i] > BOUNDS[i] && boid.new_velocity[i] > 0) ||
            (boid.position[i] < -BOUNDS[i] && boid.new_velocity[i] < 0))
        {
            boid.new_velocity[i] *= -1;
        }
}

//----------------------------------------------------------------------------------------------------------------------
// Behavior code.

PE::Vector BoidController::AvoidVector(const Boid & boid) const
{
    PE::Vector bVector{};

    for (const auto * b : boid.neighbors)
    {
        float dist = glm::distance2(b->position, boid.position);
        if (dist != 0.f)
            bVector += (boid.position - b->position) / dist;
    }
    return bVector;
}

PE::Vector BoidController::AlignVector(const Boid & boid) const
{
    PE::Vector bVector{};

    for (const auto * b : boid.neighbors)
        bVector += b->velocity;

    bVector = glm::normalize(bVector);

    return bVector;
}

PE::Vector BoidController::CohesionVector(const Boid & boid) const
{
    PE::Vector bVector{};

    for (const auto * b : boid.neighbors)
        bVector += b->position - boid.position;

    bVector = glm::normalize(bVector);
    return bVector;
}

PE::Vector BoidController::AreaVector(const Boid & boid) const
{
    PE::Vector bVector{};
    for (int i = 0; i < 3; ++i)
    {
        if (boid.position[i] > AreaMax[i] - check_dist_squared)
            bVector[i] -= 1;
        if (boid.position[i] < AreaMin[i] + check_dist_squared)
            bVector[i] += 1;
    }
    return bVector;
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
            glUniformMatrix4fv(shader->uTransform, 1, GL_FALSE, glm::value_ptr(transform_final * boid.transform * glm::scale(BoidScale)));
            glUniformMatrix4fv(shader->uModelTransform, 1, GL_FALSE,
                               glm::value_ptr(model_transform * boid.transform * glm::scale(BoidScale)));
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
        }

        glBindVertexArray(0);
    }

    PE::Graphics::LogError(__FILE__, __LINE__);
}
