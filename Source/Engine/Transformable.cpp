// All content � 2018 DigiPen (USA) Corporation, all rights reserved.
/*!
@filename Transformable.cpp
@author   Bryan Johnson
*/

#include "Transformable.h"
#include <glm/gtc/matrix_transform.hpp>

namespace PE
{
    void Transformable::Scale(float r)
    {
      Scale(r, r, r);
    }

    void Transformable::Scale(float x, float y, float z)
    {
      scale.x *= x;
      scale.y *= y;
      scale.z *= z;
      recalc_needed = true;
    }

    void Transformable::Move(float x, float y, float z)
    {
      this->Move(Vec3(x, y, z));
    }

    void Transformable::Move(Vec3 Translation)
    {
      position += Translation;
      recalc_needed = true;
    }

    void Transformable::Rotate(const Vec3 & axis, Radian angle)
    {
      rotation = glm::rotate(rotation, angle, glm::vec3(axis.x, axis.y, axis.z));
      recalc_needed = true;
    }

    void Transformable::SetRotationAxes(const Mat3 & axes)
    {
      rotation = glm::mat4x4(
              axes[0].x, axes[1].x, axes[2].x, 0,
              axes[0].y, axes[1].y, axes[2].y, 0,
              axes[0].z, axes[1].z, axes[2].z, 0,
              0, 0, 0, 1
      );
      rotation = glm::inverse(rotation);
      recalc_needed = true;
    }

    void Transformable::ResetRotation()
    {
      rotation = Mat4(1.f);
      recalc_needed = true;
    }

    void Transformable::SetScale(float r)
    {
      SetScale(r, r, r);
    }

    void Transformable::SetScale(Vec3 new_scale)
    {
      scale = new_scale;
      recalc_needed = true;
    }

    void Transformable::SetScale(float x, float y, float z)
    {
      SetScale(Vec3{x, y, z});
    }

    void Transformable::SetPosition(float x, float y, float z)
    {
      this->SetPosition(Vec3(x, y, z));
    }

    void Transformable::SetPosition(Vec3 p)
    {
      position = p;
      recalc_needed = true;
    }

    Mat4 Transformable::GetTransform()
    {
      RecalculateTransform();
      return transform;
    }

    const Mat4 & Transformable::GetTransformLocal()
    {
      RecalculateTransform();
      return transform;
    }

    void Transformable::ValidateTransform()
    {
      if (recalc_needed)
        DoRecalc();
    }

    void Transformable::CopyTransform(const Transformable & other)
    {
      *this = other;
    }

    const Mat4 & Transformable::GetTransformNoRecalc()
    {
      return transform;
    }

    void Transformable::RecalculateTransform()
    {
      if (recalc_needed)
        DoRecalc();
    }

    void Transformable::DoRecalc()
    {
      transform = glm::translate(Mat4(1.f), position) *
                  rotation *
                  glm::scale(Mat4(1.f), scale);
      recalc_needed = false;
    }

    void Transformable::SetRotation(const Mat4 & rotation_matrix)
    {
        rotation = rotation_matrix;
        recalc_needed = true;
    }
}
