// All content � 2018 DigiPen (USA) Corporation, all rights reserved.
/*!
@filename Transformable.cpp
@author   Bryan Johnson
  @brief
Transformable is a base class that acts as a wrapper for a transform matrix.
It can be inherited by other classes that need a transform, allowing them to
be manipulated easily with existing functions.
*/
#pragma once

#include "Types.h"

namespace PE
{

    /*!
    @brief Defines an object which can have a position, scale, and rotation in
             space. Designed to be a lightweight class that can be inherited by
             other classes that also want those properties.
    */
    class Transformable
    {
    public:
        Transformable() = default;

        Transformable(Vec3 position, Vec3 scale) :
                position(position),
                scale(scale),
                recalc_needed(true)
        {}

        virtual ~Transformable() = default;

        [[nodiscard]] const Vec3 & GetPosition() const
        { return position; };

        [[nodiscard]] const Vec3 & GetScale() const
        { return scale; };

        // Relative operations:

        void Scale(float r);
        void Scale(float x, float y, float z);

        void Move(float x, float y, float z);
        void Move(Vec3 translation);

        void Rotate(const Vec3 & axis, Radian angle);
        void SetRotationAxes(const Mat3 & axes);
        void SetRotation(const Mat4 & rotation_matrix);
        void ResetRotation();

        // Absolute operations:

        void SetScale(float r);
        void SetScale(Vec3 new_scale);
        void SetScale(float x, float y, float z);

        void SetPosition(float x, float y, float z);
        void SetPosition(Vec3 p);

        virtual Mat4 GetTransform();
        virtual const Mat4 & GetTransformLocal();

        void RequestRecalc()
        { recalc_needed = true; };

        void ValidateTransform();
        void CopyTransform(const Transformable & other);

    protected:

        // DO NOT USE THIS without good reason. Use GetTransform() instead.
        virtual const Mat4 & GetTransformNoRecalc();

        virtual void DoRecalc();

        // Ensures the transform matrix is correct to any recent changes.
        void RecalculateTransform();
    private:
        Vec3 position{0, 0, 0};
        Vec3 scale{1, 1, 1};
        Mat4 rotation{1};

        // Combined transform.
        Mat4 transform{};

        // Set to true whenever a paramter is changed so that
        // matrix transform can be updated when needed.
        bool recalc_needed{true};
    };
}
