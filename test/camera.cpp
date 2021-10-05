// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "camera.h"

#include "scenepic_tests.h"

namespace sp = scenepic;

int test_camera()
{
  sp::Vector center(0, 2, 0);
  sp::Vector look_at(0, 1, 0);
  sp::Vector up_dir(1, 0, 0);
  const float fov_y_degrees = 45.0f;
  const float initial_aspect_ratio = 1.5f;
  const float new_aspect_ratio = 0.9f;
  const float znear = 0.01f;
  const float zfar = 20.0f;

  sp::Transform rotation;
  rotation << 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1;

  sp::Transform world_to_camera;
  world_to_camera << 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, -2, 0, 0, 0, 1;

  sp::Transform camera_to_world;
  camera_to_world << 0, 1, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 1;

  sp::Transform projection = sp::Transforms::gl_projection(
    fov_y_degrees, initial_aspect_ratio, znear, zfar);

  int result = EXIT_SUCCESS;
  sp::Camera look_at_cam(
    center, look_at, up_dir, fov_y_degrees, znear, zfar, initial_aspect_ratio);
  test::assert_equal(look_at_cam.center(), center, result, "center");
  test::assert_equal(look_at_cam.look_at(), look_at, result, "look_at");
  test::assert_equal(look_at_cam.up_dir(), up_dir, result, "up_dir");
  test::assert_equal(look_at_cam.rotation(), rotation, result, "rotation");
  test::assert_equal(
    look_at_cam.world_to_camera(), world_to_camera, result, "world_to_camera");
  test::assert_equal(
    look_at_cam.camera_to_world(), camera_to_world, result, "camera_to_world");
  test::assert_near(
    look_at_cam.aspect_ratio(), initial_aspect_ratio, result, "aspect_ratio");
  test::assert_allclose(
    look_at_cam.projection(), projection, result, "projection");
  test::assert_equal(look_at_cam.to_json(), "camera", result);

  look_at_cam.aspect_ratio(new_aspect_ratio);
  test::assert_near(
    look_at_cam.aspect_ratio(), new_aspect_ratio, result, "aspect_ratio");

  sp::Camera rt_camera(
    center, rotation, fov_y_degrees, znear, zfar, new_aspect_ratio);
  test::assert_equal(rt_camera.center(), center, result, "center");
  test::assert_equal(rt_camera.look_at(), look_at, result, "look_at");
  test::assert_equal(rt_camera.up_dir(), up_dir, result, "up_dir");
  test::assert_equal(rt_camera.rotation(), rotation, result, "rotation");
  test::assert_equal(
    rt_camera.world_to_camera(), world_to_camera, result, "world_to_camera");
  test::assert_equal(
    rt_camera.camera_to_world(), camera_to_world, result, "camera_to_world");
  test::assert_near(
    rt_camera.aspect_ratio(), new_aspect_ratio, result, "aspect_ratio");
  rt_camera.aspect_ratio(initial_aspect_ratio);
  test::assert_allclose(
    rt_camera.projection(), projection, result, "projection");
  test::assert_equal(rt_camera.to_json(), "camera", result);

  sp::Camera camera(
    world_to_camera, fov_y_degrees, znear, zfar, new_aspect_ratio);
  test::assert_equal(camera.center(), center, result, "center");
  test::assert_equal(camera.look_at(), look_at, result, "look_at");
  test::assert_equal(camera.up_dir(), up_dir, result, "up_dir");
  test::assert_equal(camera.rotation(), rotation, result, "rotation");
  test::assert_equal(
    camera.world_to_camera(), world_to_camera, result, "world_to_camera");
  test::assert_equal(
    camera.camera_to_world(), camera_to_world, result, "camera_to_world");
  test::assert_near(
    camera.aspect_ratio(), new_aspect_ratio, result, "aspect_ratio");
  camera.aspect_ratio(initial_aspect_ratio);
  test::assert_allclose(camera.projection(), projection, result, "projection");
  test::assert_equal(camera.to_json(), "camera", result);

  return result;
}