// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_TRANSFORMS_H_
#define _SCENEPIC_TRANSFORMS_H_

#include "matrix.h"

#include <utility>

namespace scenepic
{
  namespace Transforms
  {
    /** Creates a 3D homogeneous scale matrix.
     * \param scale the scale to use across all axes
     * \return a 3D homogeneous transform matrix
     */
    Transform scale(float scale);

    /** Creates a 3D homogeneous scale matrix.
     * \param scale the axis-specific scale values
     * \return a 3D homogeneous transform matrix
     */
    Transform scale(const Vector& scale);

    /** Creates a 3D homogeneous scale matrix.
     * \param vec the movement vector
     * \return a 3D homogeneous transform matrix
     */
    Transform translate(const Vector& vec);

    /** Creates a 3D homogeneous scale matrix from axis and angle.
     * \param axis the axis of rotation
     * \param angle the angle to rotate in radians
     * \return a 3D homogeneous transform matrix
     */
    Transform rotation_matrix_from_axis_angle(const Vector& axis, float angle);

    /** Convert rotation axis and angle to a quaternion.
     * \param axis the axis of rotation
     * \param angle the angle to rotate in radians
     * \return a quaternion
     */
    Quaternion quaternion_from_axis_angle(const Vector& axis, float angle);

    /** Find the axis and angle of rotation that will ensure the x-axis
     *  aligns with the provided axis.  NB one degree of freedom
     *  is underconstrained.
     * \param axis the axis to which to align the x-axis
     * \return a 3D homogeneous transform matrix
     */
    std::pair<Vector, float> axis_angle_to_align_x_to_axis(const Vector& axis);

    /** Creates a quaternion rotation vector that will ensure the x-axis
     * aligns with the provided axis.  NB one degree of freedom is
     * underconstrained. \param axis the axis to which to align the x-axis
     * \return a quaternion
     */
    Quaternion quaternion_to_align_x_to_axis(const Vector& axis);

    /** Multiply two quaternions.
     *  \param a the first quaternion
     *  \param b the second quaternion
     *  \return the result of a * b
     */
    Quaternion quaternion_multiply(const Quaternion& a, const Quaternion& b);

    /** Creates a 3D homogeneous rotation matrix that will ensure the x-axis
     * aligns with the provided axis.  NB one degree of freedom is
     * underconstrained. \param axis the axis to which to align the x-axis
     * \return a 3D homogeneous transform matrix
     */
    Transform rotation_to_align_x_to_axis(const Vector& axis);

    /** Creates a 3D homogeneous rotation matrix about the x-axis.
     * \param angle the angle to rotate in radians
     * \return a 3D homogeneous transform matrix
     */
    Transform rotation_about_x(float angle);

    /** Creates a 3D homogeneous rotation matrix about the x-axis.
     * \param angle the angle to rotate in radians
     * \return a 3D homogeneous transform matrix
     */
    Transform rotation_about_y(float angle);

    /** Creates a 3D homogeneous rotation matrix about the x-axis.
     * \param angle the angle to rotate in radians
     * \return a 3D homogeneous transform matrix
     */
    Transform rotation_about_z(float angle);

    /** Creates a 3D homogeneous rotation matrix that will cause an object
     *  with the specified center to look at the specified location, with
     *  the up direction as given.
     *  \param center the center of the object
     *  \param look_at the position to look at
     *  \param up_dir the direction that is "up"
     *  \return a 3D homogeneous transform matrix
     */
    Transform look_at_rotation(
      const Vector& center, const Vector& look_at, const Vector& up_dir);

    /** Creates a 3D homogeneous rotation matrix from three euler angles
     *  using the specified convention. See
     *  https://en.wikipedia.org/wiki/Euler_angles
     *  for more information on valid conventions. Conventions are specified
     *  as a three character string indicating the convention order, with
     *  values taken from [X, Y, Z].
     *  \param euler_angles the Euler angles to use in constructing the matrix
     *  \param convention the convention to use in interpreting the angles
     *  \return a 3D homogeneous transform matrix
     */
    Transform euler_angles_to_matrix(
      const Vector& euler_angles, const std::string& convention = "XYZ");

    /** Creates a 3D homogeneous rotation matrix from a quaternion.
     *
     *  \param quaternion the quaternion to convert
     *  \return a 3D homogeneous transform matrix.
     */
    Transform quaternion_to_matrix(const Quaternion& quaternion);

    /** Creates a WebGL projection transform (i.e. that goes from view
     * coordinates to NDC). \param fov_y_degrees the field of view of the camera
     * in degrees \param aspect_ratio the aspect ratio of the camera \param
     * znear the z value of the near clipping plane of the frustum \param zfar
     * the z value of the far clipping plane of the frustum \return a WebGL
     * projection matrix
     */
    Transform gl_projection(
      double fov_y_degrees, double aspect_ratio, double znear, double zfar);

    /** Creates a WebGL projection transform (i.e. that goes from view
     * coordinates to NDC) from an OpenCV-style camera matrix. \param
     * camera_matrix the OpenCV-style camera matrix \param width the width of
     * the calibration image in pixels \param height the height of the
     * calibration image in pixels \param znear the z value of the near clipping
     * plane of the frustum \param zfar the z value of the far clipping plane of
     * the frustum \return a WebGL projection matrix
     */
    Transform gl_projection(
      const Intrinsic& camera_matrix,
      int width,
      int height,
      double znear,
      double zfar);

    /** Creates a WebGL view transform (i.e. that goes from world coordinates
     *  to camera view coordinates) from an OpenCV-style extrinsic matrix
     *  \param extrinsic the camera-to-world matrix, which places the camera in
     * the scene \return a WebGL view matrix
     */
    Transform gl_world_to_camera(const Extrinsic& extrinsic);

  } // namespace Transforms
} // namespace scenepic

#endif