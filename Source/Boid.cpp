// Boid code

#include <iostream>

#include "Boid.h"
#include "Engine/Dice.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/vector_angle.hpp>

bool Boid::dynamic_color = true;

float Boid::velocity_factor = 1;

static const float POP_TIME = 0.2f;


const float VEL_MAX = 1000000;
const bool DEBUG_COLORS = false;
const bool FORCE_BOUND = true;

Boid::Boid(PE::Model & model) : Model(model)
{
    static DieReal XDie(-BOUNDS.x, BOUNDS.x);
    static DieReal YDie(-BOUNDS.y, BOUNDS.y);
    static DieReal ZDie(-BOUNDS.z, BOUNDS.z);
    static DieReal VelDie(-1.f, 1.f);

    SetPosition(PE::Point(XDie.Roll(), YDie.Roll(), ZDie.Roll()));
    velocity.x = VelDie.Roll();
    velocity.y = VelDie.Roll();
    velocity.z = VelDie.Roll();
    velocity = glm::normalize(velocity);

    static DieReal PopDie(0.f, POP_TIME);
    popTimer = PopDie.Roll();
}

void Boid::Update(float dt)
{
    popTimer -= dt;
    if (popTimer < 0)
    {
        for (auto behavior : behaviors)
            behavior->PopulateTargets();
        popTimer += POP_TIME;
    }

    for (auto behavior : behaviors)
        behavior->AssessTargets();

    // Apply forces from behaviors.
    PE::Vector force{};
    for (const auto & behavior : behaviors)
        force += behavior->GetVector();

    // Find largest vector for debug colors.
    if (dynamic_color)
    {
        Behavior * largest = nullptr;
        float largest_force = 0.1;
        for (auto & bh : behaviors)
            if (glm::length(bh->GetVector()) > largest_force)
            {
                largest = bh;
                largest_force = glm::length(bh->GetVector());
            }

        if (largest)
            SetMaterial(largest->GetDebugColor());
        else
            SetMaterial(PE::white_plastic);
    }

    // If force is zero, no change in velocity.
    if (force.x == 0 && force.y == 0 && force.z == 0)
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

    // Bounce at edges.
    for (int i = 0; i < 3; ++i)
        if ((pos[i] > BOUNDS[i] && new_velocity[i] > 0) || (pos[i] < -BOUNDS[i] && new_velocity[i] < 0))
            new_velocity[i] *= -1;

    velocity = new_velocity;
    Move(velocity * dt);

    // Rotate boid to face velocity vector.
    SetRotation(glm::transpose(glm::lookAt(PE::Vec3{}, velocity, glm::normalize(pos))));

    if (DEBUG)
    {
        // Ensure boids don't go too far out of bounds.
        if (GetPosition().x > BOUNDS.x * 1.1f || GetPosition().x < -BOUNDS.x * 1.1f ||
            GetPosition().y > BOUNDS.y * 1.1f || GetPosition().y < -BOUNDS.y * 1.1f ||
            GetPosition().z > BOUNDS.z * 1.1f || GetPosition().z < -BOUNDS.z * 1.1f)
        {
            std::cout << "Boid OOB.\n";
            SetPosition(PE::Point(0, 0, 0));
        }
    }
}

Boid::~Boid()
{
    for (auto * b : behaviors)
        delete b;
}
