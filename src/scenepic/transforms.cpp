// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#define _USE_MATH_DEFINES
#include "transforms.h"

#include <Eigen/Geometry>
#include <cmath>
#include <exception>
#include <iostream>

namespace scenepic
{
  namespace Transforms
  {
    Transform scale(float scale)
    {
      Transform matrix = Transform::Identity();
      matrix(0, 0) = matrix(1, 1) = matrix(2, 2) = scale;
      return matrix;
    }

    Transform scale(const Vector& scale)
    {
      Transform matrix = Transform::Identity();
      matrix(0, 0) = scale(0);
      matrix(1, 1) = scale(1);
      matrix(2, 2) = scale(2);
      return matrix;
    }

    Transform translate(const Vector& vec)
    {
      Transform matrix = Transform::Identity();
      matrix.topRightCorner(3, 1) = vec.transpose();
      return matrix;
    }

    Transform rotation_matrix_from_axis_angle(const Vector& axis, float angle)
    {
      if (std::abs(angle) < 0.0001)
      {
        return Transform::Identity();
      }

      float x = axis(0);
      float y = axis(1);
      float z = axis(2);
      float cos = std::cos(angle);
      float sin = std::sin(angle);
      Transform matrix;
      matrix << x * x + (1 - x * x) * cos, x * y * (1 - cos) - z * sin,
        x * z * (1 - cos) + y * sin, 0, x * y * (1 - cos) + z * sin,
        y * y + (1 - y * y) * cos, y * z * (1 - cos) - x * sin, 0,
        x * z * (1 - cos) - y * sin, z * y * (1 - cos) + x * sin,
        z * z + (1 - z * z) * cos, 0, 0, 0, 0, 1;
      return matrix;
    }

    Quaternion quaternion_from_axis_angle(const Vector& axis, float angle)
    {
      Vector norm_axis = axis.normalized();
      float half_sin = std::sin(angle * 0.5f);
      float half_cos = std::cos(angle * 0.5f);
      Quaternion quat(
        norm_axis(0) * half_sin,
        norm_axis(1) * half_sin,
        norm_axis(2) * half_sin,
        half_cos);
      return quat;
    }

    std::pair<Vector, float> axis_angle_to_align_x_to_axis(const Vector& axis)
    {
      std::pair<Vector, float> axis_angle;
      Vector norm_axis = axis.normalized();
      if (norm_axis(1) == 0 && norm_axis(2) == 0)
      {
        if (norm_axis(0) == -1)
        {
          axis_angle.first << 0, 1, 0;
          axis_angle.second = static_cast<float>(M_PI);
        }
        else
        {
          axis_angle.first << 1, 0, 0;
          axis_angle.second = 0;
        }
      }
      else
      {
        float rot_angle = std::acos(norm_axis(0));
        if (rot_angle == 0)
        {
          axis_angle.first << 1, 0, 0;
          axis_angle.second = 0;
        }
        else
        {
          axis_angle.first << 0, -norm_axis(2), norm_axis(1);
          axis_angle.first /= std::sqrt(
            norm_axis(2) * norm_axis(2) + norm_axis(1) * norm_axis(1));
          axis_angle.second = rot_angle;
        }
      }

      return axis_angle;
    }

    Quaternion quaternion_to_align_x_to_axis(const Vector& axis)
    {
      auto axis_angle = axis_angle_to_align_x_to_axis(axis);
      return quaternion_from_axis_angle(axis_angle.first, axis_angle.second);
    }

    Transform rotation_to_align_x_to_axis(const Vector& axis)
    {
      auto axis_angle = axis_angle_to_align_x_to_axis(axis);
      return rotation_matrix_from_axis_angle(
        axis_angle.first, axis_angle.second);
    }

    Transform rotation_about_x(float angle)
    {
      float cos = std::cos(angle);
      float sin = std::sin(angle);
      Transform matrix;
      matrix << 1, 0, 0, 0, 0, cos, -sin, 0, 0, sin, cos, 0, 0, 0, 0, 1;
      return matrix;
    }

    Transform rotation_about_y(float angle)
    {
      float cos = std::cos(angle);
      float sin = std::sin(angle);
      Transform matrix;
      matrix << cos, 0, sin, 0, 0, 1, 0, 0, -sin, 0, cos, 0, 0, 0, 0, 1;
      return matrix;
    }

    Transform rotation_about_z(float angle)
    {
      float cos = std::cos(angle);
      float sin = std::sin(angle);
      Transform matrix;
      matrix << cos, -sin, 0, 0, sin, cos, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
      return matrix;
    }

    Transform look_at_rotation(
      const Vector& center, const Vector& look_at, const Vector& up_dir)
    {
      Transform matrix = Transform::Identity();
      auto z_axis = (center - look_at).normalized();
      auto x_axis = (up_dir.cross(z_axis)).normalized();
      auto y_axis = (z_axis.cross(x_axis)).normalized();
      matrix.row(0).leftCols(3) = x_axis;
      matrix.row(1).leftCols(3) = y_axis;
      matrix.row(2).leftCols(3) = z_axis;
      return matrix;
    }

    Transform euler_angles_to_matrix(
      const Vector& euler_angles, const std::string& convention)
    {
      Transform matrix = Transform::Identity();
      for (auto i = 2; i >= 0; --i)
      {
        auto axis = convention[i];
        float angle = euler_angles(i);
        switch (axis)
        {
          case 'X':
          case 'x':
            matrix = rotation_about_x(angle) * matrix;
            break;

          case 'Y':
          case 'y':
            matrix = rotation_about_y(angle) * matrix;
            break;

          case 'Z':
          case 'z':
            matrix = rotation_about_z(angle) * matrix;
            break;

          default:
            throw std::invalid_argument("Invalid convention: " + convention);
        }
      }

      return matrix;
    }

    Transform quaternion_to_matrix(const Quaternion& quaternion)
    {
      float qw = quaternion.w();
      float qx = quaternion.x();
      float qy = quaternion.y();
      float qz = quaternion.z();
      float qx2 = qx * qx;
      float qy2 = qy * qy;
      float qz2 = qz * qz;
      Transform matrix;
      matrix << 1 - 2 * qy2 - 2 * qz2, 2 * qx * qy - 2 * qz * qw,
        2 * qx * qz + 2 * qy * qw, 0, 2 * qx * qy + 2 * qz * qw,
        1 - 2 * qx2 - 2 * qz2, 2 * qy * qz - 2 * qx * qw, 0,
        2 * qx * qz - 2 * qy * qw, 2 * qy * qz + 2 * qx * qw,
        1 - 2 * qx2 - 2 * qy2, 0, 0, 0, 0, 1;
      return matrix;
    }

    Quaternion quaternion_multiply(const Quaternion& a, const Quaternion& b)
    {
      float x = a.w() * b.x() + a.x() * b.w() + a.y() * b.z() - a.z() * b.y();
      float y = a.w() * b.y() + a.y() * b.w() + a.z() * b.x() - a.x() * b.z();
      float z = a.w() * b.z() + a.z() * b.w() + a.x() * b.y() - a.y() * b.x();
      float w = a.w() * b.w() - a.x() * b.x() - a.y() * b.y() - a.z() * b.z();
      return Quaternion(x, y, z, w);
    }

    Transform gl_projection(
      double fov_y_degrees, double aspect_ratio, double znear, double zfar)
    {
      double fov_y = (M_PI * fov_y_degrees / 180.0);
      double f = 1.0 / std::tan(fov_y / 2);
      float fx = static_cast<float>(f / aspect_ratio);
      float fy = static_cast<float>(f);
      double nf = 1.0 / (znear - zfar);
      float A = static_cast<float>((zfar + znear) * nf);
      float B = static_cast<float>(2 * zfar * znear * nf);

      Transform matrix;
      matrix << fx, 0, 0, 0, 0, fy, 0, 0, 0, 0, A, B, 0, 0, -1, 0;
      return matrix;
    }

    Transform gl_projection(
      const Intrinsic& camera_matrix,
      int width,
      int height,
      double znear,
      double zfar)
    {
      float K00 = camera_matrix(0, 0);
      float K01 = camera_matrix(0, 1);
      float K02 = camera_matrix(0, 2);
      float K11 = camera_matrix(1, 1);
      float K12 = camera_matrix(1, 2);
      float A = static_cast<float>((zfar + znear) / (znear - zfar));
      float B = static_cast<float>(2 * zfar * znear / (znear - zfar));
      Transform matrix;
      matrix << 2 * K00 / width, -2 * K01 / width, (width - 2 * K02) / width, 0,
        0, 2 * K11 / height, (2 * K12 - height) / height, 0, 0, 0, A, B, 0, 0,
        -1, 0;
      return matrix;
    }

    Transform gl_world_to_camera(const Extrinsic& extrinsic)
    {
      Transform camera_to_world =
        extrinsic * rotation_about_x(static_cast<float>(M_PI));
      return camera_to_world.inverse();
    }

  } // namespace Transforms
} // namespace scenepic