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

    virtual ~Behavior() = default;

    // Set how far away a behavior can "see".
    void SetCheckDistance(float dist)
    { check_dist_squared = dist * dist; };

    // Look through the list of possible targets to see which concern this behavior.
    virtual void PopulateTargets() = 0;

    // Check the status of known targets to update behavior vector.
    virtual void AssessTargets() = 0;

    // Gets the current vector that represents the behavior.
    [[nodiscard]] PE::Vector GetVector() const;

    [[nodiscard]] virtual const PE::Material & GetDebugColor() const = 0;
protected:

    float check_dist_squared = 0;
    float factor = 0;

    // A pointer to the list of objects this behavior is concerned with.
    const Boids * targets{nullptr};
    const Boid * parent{nullptr};

    cBoids cur_targets{};

    PE::Vector bVector{};
};

class BehaviorAvoid : public Behavior
{
public:
    BehaviorAvoid(const Boid * parent, float factor, float check_dist,
                  const std::vector<Boid *> * targets = nullptr);
    void PopulateTargets() override;
    void AssessTargets() override;

    [[nodiscard]] const PE::Material & GetDebugColor() const override
    { return PE::red_plastic; };
};

class BehaviorAlign : public Behavior
{
public:
    BehaviorAlign(const Boid * parent, float factor, float check_dist,
                  const std::vector<Boid *> * targets = nullptr);
    void PopulateTargets() override;
    void AssessTargets() override;

    [[nodiscard]] const PE::Material & GetDebugColor() const override
    { return PE::cyan_plastic; };
};

class BehaviorCohesion : public Behavior
{
public:
    BehaviorCohesion(const Boid * parent, float factor, float check_dist,
                     const std::vector<Boid *> * targets = nullptr);
    void PopulateTargets() override;
    void AssessTargets() override;

    [[nodiscard]] const PE::Material & GetDebugColor() const override
    { return PE::green_plastic; };
};

class BehaviorStayInArea : public Behavior
{
public:
    BehaviorStayInArea(const Boid * parent, float factor, float check_dist,
                       PE::Vec3 min, PE::Vec3 max);
    void PopulateTargets() override;
    void AssessTargets() override;

    [[nodiscard]] const PE::Material & GetDebugColor() const override
    { return PE::yellow_plastic; };
private:
    PE::Vec3 min, max;
};

class BehaviorStayOutArea : public Behavior
{
public:
    BehaviorStayOutArea(const Boid * parent, float factor, float check_dist,
                        PE::Point position, PE::Vector size);

    void PopulateTargets() override;
    void AssessTargets() override;

    [[nodiscard]] const PE::Material & GetDebugColor() const override
    { return PE::yellow_plastic; };
private:
    PE::Point position;
    PE::Vector size;
};
