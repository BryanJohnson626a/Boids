#pragma once

#include <vector>
#include "Engine/Types.h"
#include "CustomTypes.h"
#include "Engine/Color.h"

class Boid;
namespace PE
{
    class Transformable;
}

class Behavior
{
public:
    Behavior(const Boid * parent, float factor, float check_dist,
             const Boids * targets = nullptr);

    // Set how far away a behavior can "see".
    void SetCheckDistance(float dist)
    { check_dist_squared = dist * dist; };

    // Look through the list of possible targets to see which concern this behavior.
    virtual void PopulateTargets() = 0;

    // Check the status of known targets to update behavior vector.
    virtual void AssessTargets() = 0;

    // Gets the current vector that represents the behavior.
    PE::Vector GetVector() const;

    virtual const PE::Material & GetDebugColor() const
    { return PE::pearl; };
protected:

    float check_dist_squared;
    float factor;

    // A pointer to the list of objects this behavior is concerned with.
    const Boids * targets;
    const Boid * parent;

    cBoids cur_targets;

    PE::Vector bVector;
};

class BehaviorAvoid : public Behavior
{
public:
    BehaviorAvoid(const Boid * parent, float factor, float check_dist,
                  const std::vector<Boid *> * targets = nullptr);
    void PopulateTargets() override;
    void AssessTargets() override;

    [[nodiscard]] const PE::Material & GetDebugColor() const override
    { return PE::ruby; };
};

class BehaviorAlign : public Behavior
{
public:
    BehaviorAlign(const Boid * parent, float factor, float check_dist,
                  const std::vector<Boid *> * targets = nullptr);
    void PopulateTargets() override;
    void AssessTargets() override;

    const PE::Material & GetDebugColor() const override
    { return PE::emerald; };
};

class BehaviorCohesion : public Behavior
{
public:
    BehaviorCohesion(const Boid * parent, float factor, float check_dist,
                     const std::vector<Boid *> * targets = nullptr);
    void PopulateTargets() override;
    void AssessTargets() override;

    [[nodiscard]] const PE::Material & GetDebugColor() const override
    { return PE::turquoise; };
};

class BehaviorStayInArea : public Behavior
{
public:
    BehaviorStayInArea(const Boid * parent, float factor, float check_dist,
                       PE::Vec3 min, PE::Vec3 max);
    void PopulateTargets() override;
    void AssessTargets() override;

    [[nodiscard]] const PE::Material & GetDebugColor() const override
    { return PE::ruby; };
private:
    PE::Vec3 min, max;
};

class BehaviorStayOutArea : public Behavior
{
public:
    BehaviorStayOutArea(const Boid * parent, float factor, float check_dist, PE::Point position, PE::Vector size);
    void PopulateTargets() override;
    void AssessTargets() override;

    [[nodiscard]] const PE::Material & GetDebugColor() const override
    { return PE::brass; };
private:
    PE::Point position;
    PE::Vector size;
};
