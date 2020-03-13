#pragma once

#include <vector>
#include "Engine/Model.h"
#include "Behavior.h"

const PE::Vec3 BOUNDS{12.f, 8.f, 4.f};

class Boid : public PE::Model
{
public:
    Boid(PE::Model & model);
    virtual void Update(float dt);
    void UpdateMove(float dt);

    [[nodiscard]] const PE::Vector & GetVelocity() const
    { return velocity; };

    void AddBehavior(Behavior * behavior)
    { behaviors.push_back(behavior); };

    void SetSpeed(float new_speed)
    { speed = new_speed; };

    void SetTurnForce(float new_turn_force)
    { turnForce = new_turn_force; };
private:
    float speed = 1;
    float turnForce = 2;

    PE::Vector velocity{};
    PE::Vector new_velocity{};

    static float velocity_factor, cohesion_factor, alignment_factor, separation_factor, avoid_factor;
    static bool dynamic_color;
    static int edge_mode;

    float popTimer{};

    std::vector<Behavior *> behaviors;
};

