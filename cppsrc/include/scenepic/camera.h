// ----------------------------------------------------------------------------
//
// camera.h -- A camera type.
//
// Copyright (C) 2021 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#ifndef _SCENEPIC_CAMERA_H_
#define _SCENEPIC_CAMERA_H_

#include "json_value.h"
#include "matrix.h"
#include "transforms.h"

namespace scenepic
{
/** A camera type */
class Camera
{
public:
    /** Constructor.
     *  \param center the camera center in world coordinates
     *  \param rotation the camera rotation matrix
     *  \param fov_y_degrees vertical field of view in degrees
     *  \param near_crop_distance the near clipping plane of the view frustum
     *  \param far_crop_distance the far clipping plane of the view frustum
     *  \param aspect_ratio the aspect ratio of the image
     */
    Camera(const Vector &center,
           const Transform &rotation,
           double fov_y_degrees = 45.0,
           double near_crop_distance = 0.01,
           double far_crop_distance = 20.0,
           double aspect_ratio = 1.0);

    /** Constructor.
     *  \param world_to_camera the world-to-camera transform.
     *  \param fov_y_degrees vertical field of view in degrees
     *  \param near_crop_distance the near clipping plane of the view frustum
     *  \param far_crop_distance the far clipping plane of the view frustum
     *  \param aspect_ratio the aspect ratio of the image
     */
    Camera(const Transform &world_to_camera,
           double fov_y_degrees = 45.0,
           double near_crop_distance = 0.01,
           double far_crop_distance = 20.0,
           double aspect_ratio = 1.0);

    /** Constructor.
     *  \param world_to_camera the world-to-camera transform.
     *  \param projection matrix translating view coordinates to NDC
     */
    Camera(const Transform &world_to_camera,
           const Transform &projection);

    /** Constructor.
     *  \param center the camera center
     *  \param look_at where the camera is looking
     *  \param up_dir the direction that is "up" for the camera
     *  \param fov_y_degrees vertical field of view in degrees
     *  \param near_crop_distance the near clipping plane of the view frustum
     *  \param far_crop_distance the far clipping plane of the view frustum
     *  \param aspect_ratio the aspect ratio of the image
     */
    Camera(const Vector &center = Vector(0, 0, 4),
           const Vector &look_at = Vector(0, 0, 0),
           const Vector &up_dir = Vector(0, 1, 0),
           double fov_y_degrees = 45.0,
           double near_crop_distance = 0.01,
           double far_crop_distance = 20.0,
           double aspect_ratio = 1.0);

    /** The camera rotation matrix */
    Transform rotation() const;

    /** The camera center */
    Vector center() const;

    /** Where the camera is looking */
    Vector look_at() const;

    /** The direction that is "up" for the camera */
    Vector up_dir() const;

    /** The aspect ratio of the camera projection. */
    float aspect_ratio() const;

    /** The aspect ratio of the camera projection. */
    Camera& aspect_ratio(float aspect_ratio);

    /** The transform placing the camera in the scene */
    const Transform &camera_to_world() const;

    /** The transform placing the world in camera coordinates */
    const Transform &world_to_camera() const;

    /** The GL project matrix, transforming from camera view coordinates to NDC */
    const Transform &projection() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    virtual JsonValue to_json() const;

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Whether this is a "no camera" instance */
    bool is_none() const;

    /** A value indicating "no camera" */
    static Camera None();

protected:
    Transform m_camera_to_world;
    Transform m_world_to_camera;
    Transform m_projection;
};


} // namespace scenepic

#endif