// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "camera.h"

#include "transforms.h"
#include "util.h"

#include <Eigen/Geometry>
#include <Eigen/LU>
#include <cmath>
#include <iostream>
#include <limits>

namespace scenepic
{
  void check_valid_rotation(const Transform& rotation, float tolerance = 1e-6f)
  {
    auto ortho_diff = (rotation * rotation.transpose() - Transform::Identity())
                        .array()
                        .abs()
                        .maxCoeff();
    auto det_diff = std::abs(rotation.determinant() - 1.0f);
    if (ortho_diff > tolerance)
    {
      std::cerr << "camera rotation: not orthogonal within tolerance of "
                << tolerance << ": " << ortho_diff << std::endl;
    }

    if (det_diff > tolerance)
    {
      std::cerr << "camera rotation: |det " << rotation.determinant()
                << " - 1| > " << tolerance << std::endl;
    }
  }

  Camera::Camera(
    const Vector& center,
    const Vector& look_at,
    const Vector& up_dir,
    double fov_y_degrees,
    double near_crop_distance,
    double far_crop_distance,
    double aspect_ratio)
  {
    auto rotation = Transforms::look_at_rotation(center, look_at, up_dir);
    auto translation = Transforms::translate(-center);
    m_world_to_camera = rotation * translation;
    m_camera_to_world = m_world_to_camera.inverse();
    m_projection = Transforms::gl_projection(
      fov_y_degrees, aspect_ratio, near_crop_distance, far_crop_distance);
  }

  Camera::Camera(
    const Vector& center,
    const Transform& rotation,
    double fov_y_degrees,
    double near_crop_distance,
    double far_crop_distance,
    double aspect_ratio)
  {
    check_valid_rotation(rotation);
    auto translation = Transforms::translate(-center);
    m_world_to_camera = rotation * translation;
    m_camera_to_world = m_world_to_camera.inverse();
    m_projection = Transforms::gl_projection(
      fov_y_degrees, aspect_ratio, near_crop_distance, far_crop_distance);
  }

  Camera::Camera(
    const Transform& world_to_camera,
    double fov_y_degrees,
    double near_crop_distance,
    double far_crop_distance,
    double aspect_ratio)
  : m_world_to_camera(world_to_camera)
  {
    if (!m_world_to_camera.isZero())
    {
      check_valid_rotation(this->rotation());
      m_camera_to_world = m_world_to_camera.inverse();
    }

    m_projection = Transforms::gl_projection(
      fov_y_degrees, aspect_ratio, near_crop_distance, far_crop_distance);
  }

  Camera::Camera(const Transform& world_to_camera, const Transform& projection)
  : m_world_to_camera(world_to_camera), m_projection(projection)
  {
    if (!m_world_to_camera.isZero())
    {
      check_valid_rotation(this->rotation());
      m_camera_to_world = m_world_to_camera.inverse();
    }
  }

  Vector Camera::center() const
  {
    Vector center = m_camera_to_world.topRightCorner(3, 1).transpose();
    return center;
  }

  Vector Camera::look_at() const
  {
    Vector look_at(0, 0, -1);
    look_at =
      (look_at.homogeneous() * m_camera_to_world.transpose()).hnormalized();
    return look_at;
  }

  Vector Camera::up_dir() const
  {
    Vector up_dir(0, 1, 0);
    up_dir = (up_dir.homogeneous() * this->rotation()).hnormalized();
    return up_dir;
  }

  Transform Camera::rotation() const
  {
    Transform rotation = m_world_to_camera;
    rotation.topRightCorner(3, 1).fill(0);
    return rotation;
  }

  JsonValue Camera::to_json() const
  {
    JsonValue obj;
    obj["CommandType"] = "SetCamera";
    obj["Value"]["WorldToCamera"] = matrix_to_json(m_world_to_camera);
    obj["Value"]["Projection"] = matrix_to_json(m_projection);
    return obj;
  }

  bool Camera::is_none() const
  {
    return m_projection.isZero() && m_world_to_camera.isZero();
  }

  Camera Camera::None()
  {
    Transform world_to_camera = Transform::Zero();
    Transform projection = Transform::Zero();
    return Camera(world_to_camera, projection);
  }

  std::string Camera::to_string() const
  {
    return this->to_json().to_string();
  }

  const Transform& Camera::world_to_camera() const
  {
    return m_world_to_camera;
  }

  const Transform& Camera::camera_to_world() const
  {
    return m_camera_to_world;
  }

  const Transform& Camera::projection() const
  {
    return m_projection;
  }

  float Camera::aspect_ratio() const
  {
    return m_projection(1, 1) / m_projection(0, 0);
  }

  Camera& Camera::aspect_ratio(float aspect_ratio)
  {
    m_projection(0, 0) = m_projection(1, 1) / aspect_ratio;
    return *this;
  }

  std::vector<Camera> Camera::orbit(
    int num_frames,
    float distance,
    int num_times,
    float min_altitude,
    float max_altitude,
    Vector up_dir,
    Vector forward_dir,
    double fov_y_degrees,
    double aspect_ratio,
    double near_crop_distance,
    double far_crop_distance)
  {
    Vector right_dir = up_dir.cross(forward_dir);
    Eigen::VectorXf azimuth(num_frames);
    Eigen::VectorXf altitude(num_frames);
    azimuth.setLinSpaced(0, num_times * 2 * EIGEN_PI);
    int half_frames = num_frames / 2;
    altitude.topRows(half_frames + 1).setLinSpaced(min_altitude, max_altitude);
    altitude.bottomRows(half_frames).setLinSpaced(min_altitude, max_altitude);
    altitude.bottomRows(half_frames).reverseInPlace();

    Transform projection = Transforms::gl_projection(
      fov_y_degrees, aspect_ratio, near_crop_distance, far_crop_distance);

    Transform init_ext = Transforms::look_at_rotation(
      -forward_dir * distance, Vector::Zero(), up_dir);
    init_ext = Transforms::translate(-forward_dir * distance) * init_ext;
    std::vector<Camera> cameras;
    for (int i = 0; i < num_frames; ++i)
    {
      Transform elevate =
        Transforms::rotation_matrix_from_axis_angle(right_dir, altitude[i]);
      Transform rotate =
        Transforms::rotation_matrix_from_axis_angle(up_dir, azimuth[i]);
      Transform camera_to_world = rotate * elevate * init_ext;
      Transform world_to_camera = camera_to_world.inverse();
      cameras.push_back(Camera(world_to_camera, projection));
    }

    return cameras;
  }

} // namespace scenepic