#include <limits>
#include <cmath>
#include <iostream>
#include <Eigen/LU>
#include <Eigen/Geometry>

#include "camera.h"
#include "transforms.h"
#include "util.h"


namespace scenepic
{
void check_valid_rotation(const Transform& rotation, float tolerance=1e-6f)
{
    auto ortho_diff = (rotation * rotation.transpose() - Transform::Identity()).array().abs().maxCoeff();
    auto det_diff = std::abs(rotation.determinant() - 1.0f);
    if(ortho_diff > tolerance)
    {
        std::cerr << "camera rotation is not orthogonal: " << ortho_diff << std::endl;
    }

    if(det_diff > tolerance)
    {
        std::cerr << "camera rotation determinant " << rotation.determinant() << " != 1" << std::endl;
    }
}

Camera::Camera(const Vector &center, const Vector &look_at,
               const Vector &up_dir, double fov_y_degrees,
               double near_crop_distance, double far_crop_distance,
               double aspect_ratio)
{
    auto rotation = Transforms::look_at_rotation(center, look_at, up_dir);
    auto translation = Transforms::translate(-center);
    this->m_world_to_camera = rotation * translation;
    this->m_camera_to_world = this->m_world_to_camera.inverse();
    this->m_projection = Transforms::gl_projection(fov_y_degrees, aspect_ratio, near_crop_distance, far_crop_distance);
}

Camera::Camera(const Vector &center, const Transform &rotation, double fov_y_degrees,
               double near_crop_distance, double far_crop_distance, double aspect_ratio)
{
    check_valid_rotation(rotation);
    auto translation = Transforms::translate(-center);
    this->m_world_to_camera = rotation * translation;
    this->m_camera_to_world = this->m_world_to_camera.inverse();
    this->m_projection = Transforms::gl_projection(fov_y_degrees, aspect_ratio, near_crop_distance, far_crop_distance);
}

Camera::Camera(const Transform &world_to_camera, double fov_y_degrees,
               double near_crop_distance, double far_crop_distance,
               double aspect_ratio) : m_world_to_camera(world_to_camera)
{    
    if(!this->m_world_to_camera.isZero())
    {
        check_valid_rotation(this->rotation());
        this->m_camera_to_world = this->m_world_to_camera.inverse();
    }

    this->m_projection = Transforms::gl_projection(fov_y_degrees, aspect_ratio, near_crop_distance, far_crop_distance);
}

Camera::Camera(const Transform &world_to_camera, const Transform &projection) : m_world_to_camera(world_to_camera), m_projection(projection)
{
    if(!this->m_world_to_camera.isZero())
    {
        check_valid_rotation(this->rotation());
        this->m_camera_to_world = this->m_world_to_camera.inverse();
    }
}

Vector Camera::center() const
{
    Vector center = this->m_camera_to_world.topRightCorner(3, 1).transpose();
    return center;
}

Vector Camera::look_at() const
{
    Vector look_at(0, 0, -1);
    look_at = (look_at.homogeneous() * this->m_camera_to_world.transpose()).hnormalized();
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
    Transform rotation = this->m_world_to_camera;
    rotation.topRightCorner(3, 1).fill(0);
    return rotation;
}

JsonValue Camera::to_json() const
{
    JsonValue obj;
    obj["CommandType"] = "SetCamera";
    obj["Value"]["WorldToCamera"] = matrix_to_json(this->m_world_to_camera);
    obj["Value"]["Projection"] = matrix_to_json(this->m_projection);
    return obj;
}

bool Camera::is_none() const
{
    return this->m_projection.isZero() && this->m_world_to_camera.isZero();
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

const Transform &Camera::world_to_camera() const
{
    return this->m_world_to_camera;
}

const Transform &Camera::camera_to_world() const
{
    return this->m_camera_to_world;
}

const Transform &Camera::projection() const
{
    return this->m_projection;
}

float Camera::aspect_ratio() const
{
    return this->m_projection(1, 1) / this->m_projection(0, 0);
}

Camera& Camera::aspect_ratio(float aspect_ratio)
{

    this->m_projection(0, 0) = this->m_projection(1, 1) / aspect_ratio;
    return *this;
}

} // namespace scenepic