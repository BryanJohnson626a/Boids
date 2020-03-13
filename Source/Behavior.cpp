
#include "Behavior.h"
#include "Boid.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

Behavior::Behavior(const Boid * parent, float factor, float check_dist,
                   const Boids * targets) :
        parent(parent),
        factor(factor),
        check_dist_squared(check_dist * check_dist),
        targets(targets)
{}

PE::Vector Behavior::GetVector() const
{
    return bVector * factor;
}

void BehaviorAvoid::PopulateTargets()
{
    cur_targets.clear();
    for (const auto * b : *targets)
    {
        float distance_squared = glm::distance2(b->GetPosition(), parent->GetPosition());
        if (b != parent && distance_squared < check_dist_squared)
            cur_targets.push_back(b);
    }
}

void BehaviorAvoid::AssessTargets()
{
    bVector = PE::Vector();

    for (const auto * b : cur_targets)
    {
        float dist = glm::distance2(b->GetPosition(), parent->GetPosition());
        if (dist != 0.f)
            bVector += (parent->GetPosition() - b->GetPosition()) / dist;
    }
}

BehaviorAvoid::BehaviorAvoid(const Boid * parent, float factor, float check_dist,
                             const Boids * targets) :
        Behavior(parent, factor, check_dist, targets)
{}

void BehaviorAlign::PopulateTargets()
{
    cur_targets.clear();
    for (const auto * b : *targets)
        if (b != parent && glm::distance2(b->GetPosition(), parent->GetPosition()) < check_dist_squared)
            cur_targets.emplace_back(b);
}

void BehaviorAlign::AssessTargets()
{
    bVector = PE::Vector();

    for (const auto * b : cur_targets)
        bVector += b->GetVelocity();

    if (!cur_targets.empty())
        bVector = glm::normalize(bVector);

}

BehaviorAlign::BehaviorAlign(const Boid * parent, float factor, float check_dist,
                             const Boids * targets) : Behavior(parent, factor, check_dist,
                                                                                          targets)
{

}

void BehaviorCohesion::PopulateTargets()
{
    cur_targets.clear();
    for (const auto * b : *targets)
        if (b != parent && glm::distance2(b->GetPosition(), parent->GetPosition()) < check_dist_squared)
            cur_targets.emplace_back(b);
}

void BehaviorCohesion::AssessTargets()
{
    bVector = PE::Vector();

    for (const auto * b : cur_targets)
        bVector += b->GetPosition() - parent->GetPosition();

    if (!cur_targets.empty())
        bVector = glm::normalize(bVector);
}

BehaviorCohesion::BehaviorCohesion(const Boid * parent, float factor, float check_dist,
                                   const Boids * targets) : Behavior(parent, factor,
                                                                                                check_dist,
                                                                                                targets)
{

}

BehaviorStayInArea::BehaviorStayInArea(const Boid * parent, float factor, float check_dist,
                                       float minX, float maxX, float minY, float maxY) :
        Behavior(parent, factor, sqrt(check_dist)), minX(minX), maxX(maxX), minY(minY), maxY(maxY)
{}

void BehaviorStayInArea::PopulateTargets()
{}

void BehaviorStayInArea::AssessTargets()
{
    bVector = PE::Vector();
    const PE::Point & pos = parent->GetPosition();

    if (pos.x > maxX - check_dist_squared)
        bVector += PE::Vector(-1, 0,0);
    if (pos.x < minX + check_dist_squared)
        bVector += PE::Vector(1, 0,0);

    if (pos.y > maxY - check_dist_squared)
        bVector += PE::Vector(0, -1,0);
    if (pos.y < minY + check_dist_squared)
        bVector += PE::Vector(0, 1,0);
}

BehaviorStayOutArea::BehaviorStayOutArea(const Boid * parent, float factor, float check_dist, PE::Point position,
                                         const PE::Vector size) :
        Behavior(parent, factor, sqrt(check_dist)), position(position), size(size)
{}

void BehaviorStayOutArea::PopulateTargets()
{}

void BehaviorStayOutArea::AssessTargets()
{
    bVector = PE::Vector();
    const PE::Point & boidPos = parent->GetPosition();

    if (boidPos.x < position.x + size.x + check_dist_squared
        && boidPos.x > position.x - size.x - check_dist_squared
        && boidPos.y < position.y + size.y + check_dist_squared
        && boidPos.y > position.y - size.y - check_dist_squared)
    {
        bVector = boidPos - position;
        bVector = glm::normalize(bVector);
    }
}
