// Boid code

#include <iostream>

#include "Boid.h"
#include "Engine/Dice.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/vector_angle.hpp>

// 1 bounce, 2 wrap.
int Boid::edge_mode = 1;

bool Boid::dynamic_color = true;

float Boid::velocity_factor = 1;

const float VEL_MAX = 1000000;
const bool DEBUG_COLORS = false;
const bool FORCE_BOUND = true;

Boid::Boid(PE::Model & model) : Model(model)
{
    static DieReal XDie(-BOUNDS.x, BOUNDS.x);
    static DieReal YDie(-BOUNDS.y, BOUNDS.y);
    static DieReal ZDie(-BOUNDS.z, BOUNDS.z);
    static DieReal VelDie(-1, 1);

    SetPosition(PE::Point(XDie.Roll(), YDie.Roll(), ZDie.Roll()));
    SetScale(0.2f, 0.1f, 1);
    velocity.x = VelDie.Roll();
    velocity.y = VelDie.Roll();
    velocity.z = VelDie.Roll();
    velocity = glm::normalize(velocity);
}

void Boid::Update(float dt)
{
    static DieReal PopDie(0.1f, 0.15f);
    popTimer -= dt;
    if (popTimer < 0)
    {
        for (auto behavior : behaviors)
            behavior->PopulateTargets();
        popTimer += PopDie.Roll();
    }

    for (auto behavior : behaviors)
        behavior->AssessTargets();


    // Apply forces from behaviors.
    PE::Vector force;
    for (auto behavior : behaviors)
        force += behavior->GetVector();

    // Find largest vector for debug colors.
    if (DEBUG_COLORS)
    {
        Behavior * largest = behaviors[0];
        for (auto b1 : behaviors)
            if (b1->GetVector().length() > largest->GetVector().length())
                largest = b1;
        SetMaterial(largest->GetDebugColor());
    }

    // If force is zero, no change in velocity.
    if (force == PE::Vec3{})
    {
        new_velocity = velocity;
        return;
    }

    // To maintain consistent turning speed, normalize the force and apply turn factor and DT mod.
    force = glm::normalize(force);
    force *= turnForce * dt;

    // Since new velocity may be too high or low, normalize to match move speed.
    new_velocity += force;
    new_velocity = glm::normalize(new_velocity);
    new_velocity *= speed;

}

void Boid::UpdateMove(float dt)
{
    PE::Point pos = GetPosition();

    if (edge_mode == 1)
    {
        // Bounce at edges.
        for (int i = 0; i < 3; ++i)
            if ((pos.x > BOUNDS[i] && new_velocity.x > 0) || (pos.x < -BOUNDS[i] && new_velocity.x < 0))
                new_velocity[i] *= -1;

    }
    else if (edge_mode == 2)
    {
        // Wrap at edges.
        for (int i = 0; i < 3; ++i)
        {
            if (pos[i] > BOUNDS[i])
                pos[i] -= 2 * BOUNDS[i];
            else if (pos[i] < -BOUNDS[i])
                pos[i] += 2 * BOUNDS[i];
        }

        SetPosition(pos);
    }

    velocity = new_velocity;
    Move(velocity * dt);

    SetRotation(glm::lookAt(pos, velocity, PE::Up));


    // Ensure boids don't go too far out of bounds.
    if (FORCE_BOUND && (GetPosition().x > BOUNDS.x * 1.1f || GetPosition().x < -BOUNDS.x * 1.1f ||
                        GetPosition().y > BOUNDS.y * 1.1f || GetPosition().y < -BOUNDS.y * 1.1f||
                        GetPosition().z > BOUNDS.z * 1.1f || GetPosition().z < -BOUNDS.z * 1.1f))
    {
        std::cout << "Boid OOB.\n";
        SetPosition(PE::Point(0, 0, 0));
    }
}
