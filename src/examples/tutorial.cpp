// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// ----------------------------------------------------------------------------
//
// C++ ScenePic Tutorials
//
// These tutorials (which acts as a C++ equivalent to the Python Tutorials)
// provides practical examples that highlight most of the functionality
// supported by ScenePic. While by no means exhaustive, these tutorials
// should form a solid start towards building useful and insightful 3D
// visualizations of your own. If there is something you feel is missing
// from this tutorial, or if there is something you would like to contribute,
// please contact the maintainers via GitHub Issues.
//
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include "scenepic.h"

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace sp = scenepic;

int randint(int max)
{
  return std::rand() % max;
}

float randf(float min = 0.0f, float max = 1.0f)
{
  float range = max - min;
  return (range * std::rand()) / static_cast<float>(RAND_MAX) + min;
}

void scene_and_canvas_basics()
{
  std::cout << "== Scene and Canvas Basics ==" << std::endl;

  // Create a Scene, the top level container in ScenePic
  sp::Scene scene;

  // A Scene can contain many Canvases
  // For correct operation, you should create these using scene1.create_canvas()
  // (rather than constructing directly using sp.Canvas(...))
  scene.create_canvas_3d("300", 300, 300);
  scene.create_canvas_3d("100", 100, 300);

  // Once we're done, we can save the ScenePic to a standalone HTML file
  scene.save_as_html("scene_and_canvas_basics.html", "Scene and Canvas Basics");
}

void meshes_and_frames()
{
  std::cout << "== Meshes and Frames ==" << std::endl;

  // Create a scene
  sp::Scene scene;

  // A Mesh is a vertex/triangle/line buffer with convenience methods
  // Meshes "belong to" the Scene, so should be created using create_mesh()
  // Meshes can be re-used across multiple frames/canvases
  auto mesh = scene.create_mesh();
  mesh->shared_color(sp::Color(
    1,
    0,
    1)); // If shared_color is not provided, you can use per - vertex coloring
  mesh->add_cube(
    sp::Color::None(),
    sp::Transforms::scale(0.1f)); // Adds a unit cube centered at the origin
  mesh->add_cube(
    sp::Color::None(),
    sp::Transforms::translate({-1, 1, -1}) * sp::Transforms::scale(0.5f));
  mesh->add_sphere(sp::Color::None(), sp::Transforms::translate({1, 1, 1}));

  // A Canvas is a 3D rendering panel
  auto canvas = scene.create_canvas_3d("", 300, 300);

  // Create an animation with multiple Frames
  // A Frame references a set of Meshes
  // Frames are created from the Canvas not the Scene
  for (int i = 0; i < 10; ++i)
  {
    auto frame = canvas->create_frame();
    frame->add_mesh(mesh, sp::Transforms::translate({i / 10.0f, 0, 0}));
    auto mesh2 = scene.create_mesh();
    mesh2->shared_color(sp::Color(1, 0, 0)).camera_space(true);
    mesh2->add_cube(
      sp::Color::None(),
      sp::Transforms::translate({0, 0, -5}) * sp::Transforms::scale(0.5f));
    frame->add_mesh(mesh2);
    auto label = scene.create_label("", "Hi", sp::Colors::White, 80);
    label->offset_distance(0.6f).camera_space(true);
    frame->add_label(label, {0, 0, -5});
  }

  scene.save_as_html("meshes_and_frames.html", "Meshes and Frames");
}

void point_clouds_1()
{
  std::cout << "== Point Clouds 1 ==" << std::endl;

  // Create a scene
  sp::Scene scene;

  // Create a mesh that we'll turn in to a point-cloud using enable_instancing()
  auto mesh = scene.create_mesh();
  mesh->shared_color(sp::Color(0, 1, 0));
  mesh->add_cube();
  mesh->apply_transform(sp::Transforms::scale(0.01f));
  sp::VectorBuffer positions = sp::random<sp::VectorBuffer>(10000, 3, 0, 1);
  positions = (2 * positions).array() - 1;
  mesh->enable_instancing(positions);

  // Create Canvas and Frame, and add Mesh to Frame
  auto canvas = scene.create_canvas_3d("", 300, 300);
  canvas->shading(sp::Shading(sp::Colors::White));
  auto frame = canvas->create_frame();
  frame->add_mesh(mesh);

  scene.save_as_html("point_clouds_1.html", "Point Clouds 1");
}

void point_clouds_2()
{
  std::cout << "== Point Clouds 2 ==" << std::endl;

  // Note that the point cloud primitive can be arbitrarily complex.
  // The primitive geometry will only be stored once for efficiency.

  // Some parameters
  float disc_thickness = 0.2f;
  float normal_length = 1.5f;
  float point_size = 0.1f;

  // A helper Mesh which we won't actually use for rendering - just to find the
  // points and normals on a sphere to be used in mesh2 below NB this is created
  // using the sp.Mesh() constructor directly so it doesn't get added
  // automatically to the Scene
  sp::Mesh sphere_mesh;
  sphere_mesh.add_sphere(sp::Color(1, 0, 0), sp::Transforms::scale(2));
  auto num_vertices = sphere_mesh.count_vertices();
  auto points = sphere_mesh.vertex_positions();
  auto normals = sphere_mesh.vertex_normals();

  // Convert the normals into quaternion rotations
  sp::QuaternionBuffer rotations = sp::QuaternionBuffer::Zero(num_vertices, 4);
  for (int i = 0; i < num_vertices; ++i)
  {
    rotations.row(i) =
      sp::Transforms::quaternion_to_align_x_to_axis(normals.row(i));
  }

  // Generate some random colors
  sp::ColorBuffer colors = sp::random<sp::ColorBuffer>(num_vertices, 3, 0, 1);

  // Create a scene
  sp::Scene scene;

  // Create a mesh that we'll turn in to a point-cloud using enable_instancing()
  auto mesh = scene.create_mesh();
  mesh->shared_color(sp::Color(0, 1, 0)).double_sided(true);

  // Add the primitive to the Mesh - a disc and a thickline showing the normal
  mesh->add_disc(
    sp::Color::None(), sp::Transforms::scale({disc_thickness, 1, 1}), 20);
  mesh->add_thickline(
    sp::Color::None(),
    {disc_thickness * 0.5f, 0, 0},
    {normal_length, 0, 0},
    0.2f,
    0.1f);
  mesh->apply_transform(sp::Transforms::scale(point_size));

  // Now turn the mesh into a point-cloud
  mesh->enable_instancing(points, rotations, colors);

  // Create Canvas and Frame, and add Mesh to Frame
  auto canvas = scene.create_canvas_3d("", 300, 300);
  auto frame = canvas->create_frame();
  frame->add_mesh(mesh);

  scene.save_as_html("point_clouds_2.html", "Point Clouds 2");
}

void misc_meshes()
{
  std::cout << "== Miscellaneous Meshes ==" << std::endl;

  // Scene is the top level container in ScenePic
  sp::Scene scene;

  // Ok - let's start by creating some Mesh objects

  // Mesh 1 - contains a cube and a sphere
  // Mesh objects can contain arbitrary triangle mesh and line geometry
  // Meshes can belong to "layers" which can be controlled by the user
  // interactively
  auto mesh1 = scene.create_mesh("sphere+", "Sphere+");
  mesh1->add_cube(sp::Color(1, 0, 0), sp::Transforms::translate({-2, 0, -2}));
  mesh1->add_uv_sphere(
    sp::Color(0, 0, 1),
    sp::Transforms::translate({-1, 1, 0}) * sp::Transforms::scale(1.8f),
    10,
    10,
    false,
    true);
  mesh1->add_icosphere(
    sp::Color(0, 1, 1),
    sp::Transforms::translate({2, 1, 0}) * sp::Transforms::scale(1.8f),
    2,
    false,
    true);

  // Mesh 2 - coordinate axes
  auto mesh2 = scene.create_mesh("coords", "Coords");
  mesh2->add_coordinate_axes();

  // Mesh 3 - example of Loop Subdivision on a cube
  sp::VectorBuffer cube_verts(8, 3);
  cube_verts << -0.5, -0.5, -0.5, +0.5, -0.5, -0.5, -0.5, +0.5, -0.5, +0.5,
    +0.5, -0.5, -0.5, -0.5, +0.5, +0.5, -0.5, +0.5, -0.5, +0.5, +0.5, +0.5,
    +0.5, +0.5;

  sp::TriangleBuffer cube_tris(12, 3);
  cube_tris << 0, 2, 3, 0, 3, 1, 1, 3, 7, 1, 7, 5, 4, 5, 7, 4, 7, 6, 4, 6, 2, 4,
    2, 0, 2, 6, 7, 2, 7, 3, 4, 0, 1, 4, 1, 5;
  sp::LoopSubdivisionStencil stencil =
    sp::LoopSubdivisionStencil::create(cube_tris, 2, false);
  sp::VectorBuffer cube_verts_a = stencil.apply(cube_verts);
  sp::TriangleBuffer cube_tris_a = stencil.triangles();

  stencil = sp::LoopSubdivisionStencil::create(cube_tris, 2, true);
  sp::VectorBuffer cube_verts_b = stencil.apply(cube_verts);
  sp::TriangleBuffer cube_tris_b = stencil.triangles();

  auto mesh3 = scene.create_mesh();
  mesh3->shared_color(sp::Color(1, 0.8f, 0.8f));
  mesh3->add_mesh_without_normals(
    cube_verts,
    cube_tris,
    sp::ColorBufferNone(),
    sp::UVBufferNone(),
    sp::Transforms::translate({-1, 0, 0}));
  mesh3->add_mesh_without_normals(cube_verts_a, cube_tris_a);
  mesh3->add_mesh_without_normals(
    cube_verts_b,
    cube_tris_b,
    sp::ColorBufferNone(),
    sp::UVBufferNone(),
    sp::Transforms::translate({1, 0, 0}));

  // Mesh 4 - line example
  auto mesh4 = scene.create_mesh();
  int num_segs = 7000;

  sp::VertexBuffer colored_points(num_segs, 6);
  colored_points.leftCols(3) =
    sp::rowwise_cumsum(sp::random<sp::VectorBuffer>(num_segs, 3, 0, 1) * 0.2);
  colored_points.rightCols(3) = sp::random<sp::ColorBuffer>(num_segs, 3, 0, 1);

  mesh4->add_lines(
    colored_points.topRows(num_segs - 1),
    colored_points.bottomRows(num_segs - 1));
  mesh4->add_camera_frustum(sp::Color(1, 1, 0));

  // Let's create two Canvases this time
  auto canvas1 = scene.create_canvas_3d("canvas1", 300, 300);
  auto canvas2 = scene.create_canvas_3d("canvas2", 300, 300);

  // We can link their keyboard/mouse/etc. input events to keep the views in
  // sync
  scene.link_canvas_events({"canvas1", "canvas2"});

  // And we can specify that certain named "mesh collections" should have
  // user-controlled visibility and opacity Meshs without mesh_collection set,
  // or without specified visibilities will always be visible and opaque
  canvas1->set_layer_settings(
    {{"Coords", sp::LayerSettings().filled(false)},
     {"Sphere+", sp::LayerSettings().filled(true)}});

  // A Frame contains an array of meshes
  canvas1->create_frame(
    "", sp::FocusPoint::None(), {mesh1->mesh_id(), mesh2->mesh_id()});
  canvas2->create_frame(
    "", sp::FocusPoint::None(), {mesh2->mesh_id(), mesh3->mesh_id()});
  canvas2->create_frame(
    "", sp::FocusPoint::None(), {mesh4->mesh_id(), mesh1->mesh_id()});

  scene.save_as_html("misc_meshes.html", "Miscellaneous Meshes");
}

void images_and_textures()
{
  std::cout << "== Images and Textures ==" << std::endl;

  // Scene is the top level container in ScenePic
  sp::Scene scene;

  // Create and populate an Image object
  auto image1 = scene.create_image("PolarBear");
  image1->load("PolarBear.png"); // This will preserve the image data in
                                 // compressed PNG format

  // Create a texture map
  auto texture = scene.create_image("texture");
  texture->load("uv.png"); // we can use this image to skin meshes

  // Example of a mesh that is defined in camera space not world space
  // This will not move as the virtual camera is moved with the mouse
  auto cam_space_mesh = scene.create_mesh();
  cam_space_mesh->shared_color(sp::Color(1, 0, 0));
  cam_space_mesh->camera_space(true);
  cam_space_mesh->add_sphere(
    sp::Color::None(), sp::Transforms::translate({10, -10, -20}));

  // Some textured primitives
  auto sphere = scene.create_mesh();
  sphere->texture_id(texture->image_id());
  sphere->nn_texture(false);
  sphere->add_icosphere(sp::Color::None(), sp::Transform::Identity(), 4);

  auto cube = scene.create_mesh();
  cube->texture_id(texture->image_id());
  auto transform =
    sp::Transforms::translate({-1, 0, 0}) * sp::Transforms::scale(0.5f);
  cube->add_cube(sp::Color::None(), transform);

  // Show images in 3D canvas
  auto canvas = scene.create_canvas_3d();
  canvas->shading(sp::Shading(sp::Colors::White));
  auto mesh1 = scene.create_mesh();
  mesh1->texture_id("PolarBear");
  mesh1->add_image(); // Adds image in canonical position

  // Add an animation that rigidly transforms each image
  int n_frames = 20;
  for (int i = 0; i < n_frames; ++i)
  {
    float angle = static_cast<float>(2 * M_PI * i / n_frames);
    float cos = std::cos(angle);
    float sin = std::sin(angle);

    // Create a focus point that allows you to "lock" the camera's translation
    // and optionally orientation by pressing the "l" key
    sp::Vector axis(1, 0, 1);
    axis.normalize();
    sp::FocusPoint focus_point({cos, sin, 0}, axis * angle);

    auto mesh = scene.create_mesh();
    mesh->add_coordinate_axes(
      1,
      0.1f,
      sp::Transforms::translate(focus_point.position()) *
        sp::Transforms::rotation_matrix_from_axis_angle(axis, angle));

    auto image_b = scene.create_image();
    image_b->load("rand15x15.png");
    auto mesh_b = scene.create_mesh();
    mesh_b->texture_id(image_b->image_id());
    mesh_b->is_billboard(true);
    mesh_b->use_texture_alpha(true);
    mesh_b->add_image();
    mesh_b->apply_transform(
      sp::Transforms::scale(2.0) * sp::Transforms::translate({0, 0, -1}));

    auto frame = canvas->create_frame();
    frame->focus_point(focus_point);
    frame->add_mesh(mesh1, sp::Transforms::translate({cos, sin, 0}));
    frame->add_mesh(
      mesh_b,
      sp::Transforms::scale(i * 1.0 / n_frames) *
        sp::Transforms::translate({-cos, -sin, 0}));
    frame->add_mesh(cam_space_mesh);
    frame->add_mesh(
      sphere,
      sp::Transforms::rotation_about_y(
        static_cast<float>(M_PI * 2 * i / n_frames)));
    frame->add_mesh(
      cube,
      sp::Transforms::rotation_about_y(
        static_cast<float>(-M_PI * 2 * i / n_frames)));
    frame->add_mesh(mesh);
  }

  scene.save_as_html("images_and_textures.html", "Images and Textures");
}

void canvas_2d()
{
  std::cout << "== 2D Canvases ==" << std::endl;

  // Scene is the top level container in ScenePic
  sp::Scene scene;

  // Load some images
  auto image1 = scene.create_image("PolarBear");
  image1->load("PolarBear.png");

  auto image2 = scene.create_image("Random");
  image2->load("rand30x20.png");

  // Create a 2D canvas demonstrating different image positioning options
  auto canvas1 = scene.create_canvas_2d("", 400, 300);
  canvas1->background_color(sp::Colors::White);
  canvas1->create_frame()->add_image(image1->image_id(), "fit");
  canvas1->create_frame()->add_image(image1->image_id(), "fill");
  canvas1->create_frame()->add_image(image1->image_id(), "stretch");
  canvas1->create_frame()->add_image(
    image1->image_id(), "manual", 50, 50, 0.3f);

  // You can composite images and primitives too
  auto canvas2 = scene.create_canvas_2d("", 300, 300);
  auto frame = canvas2->create_frame();
  frame->add_image(image2->image_id(), "fit");
  frame->add_image(image1->image_id(), "manual", 30, 30, 0.2f);
  frame->add_circle(200, 200, 40, sp::Colors::Blue, 10, sp::Colors::Black);
  frame->add_rectangle(
    200, 100, 50, 25, sp::Colors::Black, 0, sp::Colors::Green);
  frame->add_text(
    "Hello World", 30, 100, sp::Colors::White, 100, "segoe ui light");

  scene.save_as_html("canvas_2d.html", "2D Canvases");
}

void opacity_and_labels()
{
  std::cout << "== Opacity and Labels ==" << std::endl;

  std::srand(55);

  sp::Scene scene;
  auto canvas = scene.create_canvas_3d("", 700, 700);
  auto frame = canvas->create_frame();

  std::string horizontal_align_options[] = {"left", "center", "right"};
  std::string vertical_align_options[] = {"top", "middle", "bottom"};

  std::map<std::string, sp::LayerSettings> layer_settings;
  layer_settings["Labels"] = sp::LayerSettings();
  int num_objects = 20;
  for (int i = 0; i < num_objects; ++i)
  {
    // Sample object
    int geotype = randint(2);
    sp::Color color = sp::random<sp::Color>(0, 1);
    float size = randf(0.2f, 0.5f);
    sp::Vector position = sp::random<sp::Vector>(-1.5f, 1.5f);
    float opacity = randint(2) == 0 ? 1.0f : randf(0.45, 0.55);

    // Generate geometry
    std::string layer_id = "Layer" + std::to_string(i);
    auto mesh = scene.create_mesh("", layer_id);
    mesh->shared_color(color);
    layer_settings[layer_id] = sp::LayerSettings();
    if (geotype)
    {
      mesh->add_sphere();
    }
    else
    {
      mesh->add_cube();
    }

    mesh->apply_transform(sp::Transforms::scale(size)); // Scale the primitive
    mesh->apply_transform(sp::Transforms::translate(position));
    frame->add_mesh(mesh);

    // Add label
    std::stringstream text;
    text.precision(2);
    text << color[0] << " " << color[1] << " " << color[2] << " " << opacity;
    std::string horizontal_align = horizontal_align_options[randint(3)];
    std::string vertical_align = vertical_align_options[randint(3)];

    float offset_distance;
    if (geotype)
    {
      if (horizontal_align != "center" && vertical_align != "middle")
      {
        offset_distance = size * 0.5f * 0.8f;
      }
      else
      {
        offset_distance = size * 0.6f;
      }
    }
    else
    {
      if (horizontal_align != "center" && vertical_align != "middle")
      {
        offset_distance = size * 0.7;
      }
      else
      {
        offset_distance = size * 0.9;
      }
    }

    auto label = scene.create_label(
      "",
      text.str(),
      color,
      80 * size,
      "consolas",
      "Labels",
      horizontal_align,
      vertical_align,
      offset_distance);
    frame->add_label(label, position);
  }

  canvas->set_layer_settings(layer_settings);

  scene.save_as_html("opacity_and_labels.html", "Opacity and Labels");
}

Eigen::VectorXf random_linspace(float min_val, float max_val, int num_samples)
{
  const float scale = max_val - min_val;
  Eigen::VectorXf values(num_samples);
  for (int i = 0; i < num_samples; ++i)
  {
    values(i) = ((scale * i) / (num_samples - 1) + min_val);
  }

  for (int i = 0; i < num_samples - 1; ++i)
  {
    int j = i + std::rand() % (num_samples - i);
    std::swap(values(i), values(j));
  }

  return values;
}

void animation0()
{
  std::cout << "== Animation ==" << std::endl;

  // let's create our scene to get started
  sp::Scene scene;
  auto canvas = scene.create_canvas_3d("jelly", 700, 700);

  // Load a mesh to animate
  auto jelly_mesh = sp::load_obj("jelly.obj");
  auto texture = scene.create_image("texture");
  texture->load("jelly.png");

  // create a base mesh for the animation. The animation
  // will only change the vertex positions, so this mesh
  // is used to set everything else, e.g. textures.
  auto base_mesh = scene.create_mesh("jelly_base");
  base_mesh->texture_id(texture->image_id()).use_texture_alpha(true);
  base_mesh->add_mesh(jelly_mesh);

  // this base mesh will be instanced, so we can animate each
  // instance individual using rigid transforms, in this case
  // just translation.  auto marbles = scene.create_mesh("marbles_base");
  int num_marbles = 10;
  auto marbles = scene.create_mesh("marbles_base");
  marbles->add_sphere(sp::Colors::White, sp::Transforms::scale(0.2));
  sp::VectorBuffer marble_positions = sp::VectorBuffer::Zero(num_marbles, 3);
  marble_positions.col(0) = random_linspace(-0.6, 0.6, num_marbles);
  marble_positions.col(2) = random_linspace(-1.0, 0.7, num_marbles);
  const float max_offset = static_cast<float>(2 * M_PI);
  Eigen::VectorXf marble_offsets =
    sp::random<Eigen::VectorXf>(num_marbles, 1, 0, max_offset);
  sp::ColorBuffer marble_colors_start =
    sp::random<sp::ColorBuffer>(num_marbles, 3, 0, 1);
  sp::ColorBuffer marble_colors_end =
    sp::random<sp::ColorBuffer>(num_marbles, 3, 0, 1);
  marbles->enable_instancing(
    marble_positions, sp::QuaternionBufferNone(), marble_colors_start);

  for (int i = 0; i < 60; ++i)
  {
    // animate the wave mesh by updating the vertex positions
    auto frame = canvas->create_frame();
    sp::VectorBuffer positions = jelly_mesh->position_buffer();
    auto delta_x = (positions.col(0).array() + 0.0838f * i) * 10.0f;
    auto delta_z = (positions.col(2).array() + 0.0419f * i) * 10.0f;
    positions.col(1) =
      positions.col(1).array() + 0.1 * (delta_x.cos() + delta_z.sin());

    // we create a mesh update with the new posiitons. We can use this mesh
    // update just like a new mesh, because it essentially is one, as ScenePic
    // will create a new mesh from the old one using these new positions.
    auto mesh_update = scene.update_mesh_positions("jelly_base", positions);
    frame->add_mesh(mesh_update);

    // this is a simpler form of animation in which we will change the position
    // and colors of the marbles
    Eigen::VectorXf marble_y = 0.105 * i + marble_offsets.array();
    positions = marble_positions;
    positions.col(1) = marble_y.array().sin();
    Eigen::VectorXf alpha = (marble_y.array().sin() + 1) * 0.5;
    Eigen::VectorXf beta = 1 - alpha.array();
    sp::ColorBuffer colors = alpha.asDiagonal() * marble_colors_start;
    colors += beta.asDiagonal() * marble_colors_end;

    auto marbles_update = scene.update_instanced_mesh(
      "marbles_base", positions, sp::QuaternionBufferNone(), colors);
    frame->add_mesh(marbles_update);
  }

  std::cout << std::endl << "Before compression:" << std::endl;
  auto command_sizes = scene.measure_command_size();
  for (auto& item : command_sizes)
  {
    std::cout << item.first << ": " << item.second << std::endl;
  }

  auto info = scene.quantize_updates();
  std::cout << info["jelly_base"].to_string() << std::endl;
  std::cout << std::endl << "compressing updates..." << std::endl;

  command_sizes = scene.measure_command_size();
  std::cout << std::endl << "After compression:" << std::endl;
  for (auto& item : command_sizes)
  {
    std::cout << item.first << ": " << item.second << std::endl;
  }

  scene.save_as_html("animation0.html", "Animation");
}

void animation1()
{
  std::cout << "== Instanced Animation ==" << std::endl;

  // In this tutorial we will explore how we can use mesh updates on
  // instanced meshes as well. We will begin by creating a simple primitive
  // and use instancing to create a cloud of stylized butterflies. We will
  // then using mesh updates on the instances to make the butterflies
  // fly.

  sp::Scene scene;

  auto butterflies = scene.create_mesh("butterflies");
  // the primitive will be a single wing, and we'll use instancing to create
  // all the butterflies
  butterflies->double_sided(true);
  butterflies->add_quad(
    sp::Colors::Blue,
    {0, 0, 0},
    {0.1, 0, 0.04},
    {0.08, 0, -0.06},
    {0.015, 0, -0.03});

  sp::Quaternion rotate_back =
    sp::Transforms::quaternion_from_axis_angle({1, 0, 0}, -M_PI / 6);

  Eigen::Index num_butterflies = 100;

  int num_anim_frames = 20;

  // this will make them flap their wings independently
  std::vector<int> start_frames(num_butterflies, 0);
  std::generate(start_frames.begin(), start_frames.end(), [num_anim_frames]() {
    return std::rand() % num_anim_frames;
  });

  Eigen::VectorXf rot_angles = Eigen::VectorXf::Random(num_butterflies);
  sp::QuaternionBuffer rotations(num_butterflies * 2, 4);
  sp::VectorBuffer positions =
    sp::random<sp::VectorBuffer>(num_butterflies * 2, 3, -1, 1);
  sp::ColorBuffer colors =
    sp::random<sp::ColorBuffer>(num_butterflies * 2, 3, 0, 1);
  for (Eigen::Index b = 0; b < num_butterflies; ++b)
  {
    auto rot =
      sp::Transforms::quaternion_from_axis_angle({0, 1, 0}, rot_angles(b));
    rotations.row(2 * b) = rotations.row(2 * b + 1) = rot;

    // we will use the second position as a destination
    float dx = std::sin(rot_angles(b)) * 0.1;
    float dy = positions(2 * b + 1, 1) - positions(2 * b, 1);
    dy = dy > 0.1 ? 0.1 : (dy < -0.1 ? -0.1 : dy);
    float dz = std::cos(rot_angles(b)) * 0.1;
    positions.row(2 * b + 1) = positions.row(2 * b) + sp::Vector(dx, dy, dz);
  }

  butterflies->enable_instancing(positions, rotations, colors);

  auto canvas = scene.create_canvas_3d("main", 700, 700);
  canvas->shading(sp::Shading(sp::Colors::White));

  float start = -M_PI / 6;
  float end = M_PI / 2;
  float delta = (end - start) / (num_anim_frames / 2 - 1);

  // let's construct the animation frame by frame
  std::vector<std::shared_ptr<sp::MeshUpdate>> animation;
  for (int i = 0; i < num_anim_frames; ++i)
  {
    sp::VectorBuffer frame_positions =
      sp::VectorBuffer::Zero(num_butterflies * 2, 3);
    sp::QuaternionBuffer frame_rotations =
      sp::QuaternionBuffer::Zero(num_butterflies * 2, 4);
    sp::ColorBuffer frame_colors =
      sp::ColorBuffer::Zero(num_butterflies * 2, 3);
    for (int b = 0; b < num_butterflies; ++b)
    {
      int frame = (i + start_frames[b]) % num_anim_frames;
      float angle;
      if (frame < num_anim_frames / 2)
      {
        angle = start + delta * frame;
      }
      else
      {
        angle = end + delta * (frame - num_anim_frames / 2);
      }

      // we create two rotations, one for each wing
      sp::Quaternion right =
        sp::Transforms::quaternion_from_axis_angle({0, 0, 1}, angle);
      right = sp::Transforms::quaternion_multiply(rotate_back, right);
      right = sp::Transforms::quaternion_multiply(rotations.row(2 * b), right);
      sp::Quaternion left =
        sp::Transforms::quaternion_from_axis_angle({0, 0, 1}, M_PI - angle);
      left = sp::Transforms::quaternion_multiply(rotate_back, left);
      left =
        sp::Transforms::quaternion_multiply(rotations.row(2 * b + 1), left);
      frame_rotations.row(2 * b) = right;
      frame_rotations.row(2 * b + 1) = left;

      float progress = std::sin((frame * 2 * M_PI) / num_anim_frames);
      progress = (progress + 1) * 0.5;

      // we move the butterfly along its path
      sp::Vector pos = (1 - progress) * positions.row(2 * b) +
        progress * positions.row(2 * b + 1);
      pos.y() -= std::sin(angle) * 0.02;
      frame_positions.row(2 * b) = frame_positions.row(2 * b + 1) = pos;

      // finally, we alter the color
      progress = (progress + 1) * 0.5;
      sp::Color color =
        (1 - progress) * colors.row(2 * b) + progress * colors.row(2 * b + 1);
      frame_colors.row(2 * b) = frame_colors.row(2 * b + 1) = color;
    }

    // now we create the update. Here we update position, rotation,
    // and color, but you can update them separately as well by passing
    // the `*None()` versions of the buffers to this function.
    auto update = scene.update_instanced_mesh(
      "butterflies", frame_positions, frame_rotations, frame_colors);
    animation.push_back(update);
  }

  // now we create the encapsulating animation which will move the camera
  // around the butterflies. The inner animation will loop as the camera moves.
  int num_frames = 300;
  std::vector<sp::Camera> cameras = sp::Camera::orbit(num_frames, 3.0, 2);
  for (int i = 0; i < num_frames; ++i)
  {
    auto frame = canvas->create_frame();
    frame->add_mesh(animation[i % num_anim_frames]);
    frame->camera(cameras[i]);
  }

  scene.save_as_html("animation1.html", "Instanced Animation");
}

void camera_movement()
{
  std::cout << "== Camera Movement ==" << std::endl;

  // in this tutorial we will show how to create per-frame camera movement.
  // while the user can always choose to override this behavior, having a
  // camera track specified can be helpful for demonstrating particular
  // items in 3D. We will also show off the flexible GLCamera class.

  sp::Scene scene;
  auto spin_canvas = scene.create_canvas_3d("spin");
  auto spiral_canvas = scene.create_canvas_3d("spiral");

  // let's create some items in the scene so we have a frame of reference
  auto polar_bear = scene.create_image("polar_bear");
  polar_bear->load("PolarBear.png");
  auto uv_texture = scene.create_image("texture");
  uv_texture->load("uv.png");

  auto cube = scene.create_mesh("cube");
  cube->texture_id(polar_bear->image_id());
  cube->add_cube();
  auto sphere = scene.create_mesh("sphere");
  sphere->texture_id(uv_texture->image_id());
  sphere->add_icosphere(
    sp::Color::None(), sp::Transforms::translate({0, 1, 0}), 4);

  int num_frames = 60;
  for (int i = 0; i < num_frames; ++i)
  {
    float angle = static_cast<float>(i * M_PI * 2 / num_frames);

    // for the first camera we will spin in place on the Z axis
    auto rotation = sp::Transforms::rotation_about_z(angle);
    sp::Camera spin_camera(sp::Vector(0, 0, 4), rotation, 30);

    // for the second camera, we will spin the camera in a spiral around the
    // scene we can do this using the look-at initialization, which provides a
    // straightforward "look at" interface for camera placement.
    sp::Vector camera_center(
      4 * std::cos(angle), i * 4.0f / num_frames - 2, 4 * std::sin(angle));
    sp::Camera spiral_camera(camera_center, sp::Vector(0, 0.5f, 0));

    // we can add frustums directly using the ScenePic camera objects
    auto frustums = scene.create_mesh();
    frustums->add_camera_frustum(spin_camera, sp::Colors::Red);
    frustums->add_camera_frustum(spiral_camera, sp::Colors::Green);

    std::vector<std::string> mesh_ids = {
      cube->mesh_id(), sphere->mesh_id(), frustums->mesh_id()};

    auto spin_frame = spin_canvas->create_frame();
    spin_frame->camera(
      spin_camera); // each frame can have its own camera object
    spin_frame->add_meshes_by_id(mesh_ids);

    auto spiral_frame = spiral_canvas->create_frame();
    spiral_frame->camera(spiral_camera);
    spiral_frame->add_meshes_by_id(mesh_ids);
  }

  scene.link_canvas_events(
    {spin_canvas->canvas_id(), spiral_canvas->canvas_id()});
  scene.save_as_html("camera_movement.html", "Camera Movement");
}

void set_audio(
  sp::Scene& scene,
  std::shared_ptr<sp::Canvas3D>& canvas,
  const std::string& path)
{
  auto audio = scene.create_audio();
  audio->load(path);
  canvas->media_id(audio->audio_id());
}

void audio_tracks()
{
  std::cout << "== Audio Tracks ==" << std::endl;

  // in this tutorial we'll show how to attach audio tracks to canvases.
  // ScenePic supports any audio file format supported by the browser.

  sp::Scene scene;

  std::vector<std::string> names = {"red", "green", "blue"};
  std::vector<sp::Color> colors = {
    sp::Colors::Red, sp::Colors::Green, sp::Colors::Blue};
  std::vector<float> frequencies = {0, 1, 0.5f};

  auto graph = scene.create_graph("graph", 600, 150, "graph");
  for (int i = 0; i < 3; ++i)
  {
    auto mesh = scene.create_mesh();
    mesh->add_cube(colors[i]);

    // each canvas can have a different audio file linked to it
    // and ScenePic will blend them into a single audio output
    auto canvas = scene.create_canvas_3d(names[i], 200, 200, names[i]);
    set_audio(scene, canvas, names[i] + ".ogg");
    std::vector<float> values;

    for (int j = 0; j < 60; ++j)
    {
      auto frame = canvas->create_frame();
      double scale = std::sin(j * 2 * M_PI * frequencies[i] / 30);
      values.push_back(static_cast<float>(scale));
      frame->add_mesh(
        mesh,
        sp::Transforms::scale(static_cast<float>((scale + 1) / 2 + 0.5f)));
    }

    graph->add_sparkline(names[i], values, colors[i]);
    graph->media_id(canvas->media_id());
  }

  names.push_back("graph");
  scene.link_canvas_events(names);
  scene.grid("600px", "1fr auto", "1fr 1fr 1fr");
  scene.place("graph", "2", "1 / span 3");
  scene.save_as_html("audio_tracks.html", "Audio Tracks");
}

const int SIZE = 400;

std::pair<float, float> angle_to_pos(float angle, float radius)
{
  int x = static_cast<int>(std::cos(angle) * radius) + SIZE / 2;
  int y = static_cast<int>(std::sin(angle) * radius) + SIZE / 2;
  return std::make_pair(static_cast<float>(x), static_cast<float>(y));
}

void circles_video()
{
  std::cout << "== Circles Video ==" << std::endl;

  // It is also possible to attach videos to ScenePic scenes. Once attached, you
  // can draw the frames of those videos to canvases in the same way as images,
  // and can draw the same video to multiple frames. Once a media file (video or
  // audio) has been attached to a canvas, that file will be used to drive
  // playback. In practical terms, this means that ScenePic will display frames
  // such that they line up with the timestamps of the video working on the
  // assumption that ScenePic frames are displayed at the framerate of the
  // video.

  sp::Scene scene;

  // A scene can have one or more videos attached to it
  auto video = scene.create_video();
  video->load("circles.mp4");

  // Attached videos works much the same way as adding audio
  auto tracking = scene.create_canvas_2d("tracking", SIZE, SIZE);
  tracking->background_color(sp::Colors::White);
  tracking->media_id(video->video_id());
  auto multi = scene.create_canvas_2d("multi", SIZE, SIZE);
  multi->background_color(sp::Colors::White);
  multi->media_id(video->video_id());

  for (int i = 0; i < 360; ++i)
  {
    // if a 2D canvas has an associated video
    // then a frame of that video can be added
    // via the add_video method.
    auto frame = tracking->create_frame();
    frame->add_video("fit", 0.0f, 0.0f, 1.0f, false);

    float angle = static_cast<float>(i * M_PI / 180.0f);
    auto red_pos = angle_to_pos(angle, 160);
    frame->add_rectangle(
      red_pos.first - 11,
      red_pos.second - 11,
      22,
      22,
      sp::Color::from_bytes(255, 0, 0),
      2,
      sp::Color::None(),
      "rect");
    frame->add_circle(
      red_pos.first,
      red_pos.second,
      10,
      sp::Color::from_bytes(255, 0, 0),
      1.0f,
      sp::Color::from_bytes(255, 0, 0),
      "dot");

    auto green_pos = angle_to_pos(-2 * angle, 80);
    frame->add_rectangle(
      green_pos.first - 11,
      green_pos.second - 11,
      22,
      22,
      sp::Color::from_bytes(0, 255, 0),
      2,
      sp::Color::None(),
      "rect");
    frame->add_circle(
      green_pos.first,
      green_pos.second,
      10,
      sp::Color::from_bytes(0, 255, 0),
      1.0f,
      sp::Color::from_bytes(0, 255, 0),
      "dot");

    auto blue_pos = angle_to_pos(4 * angle, 40);
    frame->add_rectangle(
      blue_pos.first - 11,
      blue_pos.second - 11,
      22,
      22,
      sp::Color::from_bytes(0, 0, 255),
      2,
      sp::Color::None(),
      "rect");
    frame->add_circle(
      blue_pos.first,
      blue_pos.second,
      10,
      sp::Color::from_bytes(0, 0, 255),
      1.0f,
      sp::Color::from_bytes(0, 0, 255),
      "dot");

    frame = multi->create_frame();
    frame->add_video(
      "manual", red_pos.first - 40, red_pos.second - 40, 0.2f, false, "red");
    frame->add_video(
      "manual",
      green_pos.first - 25,
      green_pos.second - 25,
      0.125f,
      false,
      "green");
    frame->add_video("manual", 160, 160, 0.2f, false, "blue");
  }

  tracking->set_layer_settings(
    {{"rect", sp::LayerSettings().render_order(0)},
     {"dot", sp::LayerSettings().render_order(1)}});

  scene.link_canvas_events({"tracking", "multi"});
  scene.save_as_html("circles_video.html", "Circles Video");
}

sp::Camera load_camera(const sp::JsonValue& camera_info)
{
  // this function loads an "OpenCV"-style camera representation
  // and converts it to a GL style for use in ScenePic

  sp::Vector location;
  location.x() = camera_info["location"].values()[0].as_float();
  location.y() = camera_info["location"].values()[1].as_float();
  location.z() = camera_info["location"].values()[2].as_float();

  sp::Vector euler_angles;
  euler_angles.x() = camera_info["rotation"].values()[0].as_float();
  euler_angles.y() = camera_info["rotation"].values()[1].as_float();
  euler_angles.z() = camera_info["rotation"].values()[2].as_float();

  float fov = camera_info["fov"].as_float();
  float aspect_ratio =
    camera_info["width"].as_float() / camera_info["height"].as_float();

  auto rotation = sp::Transforms::euler_angles_to_matrix(euler_angles, "XYZ");
  auto translation = sp::Transforms::translate(location);
  auto extrinsics = (translation * rotation).eval();
  auto world_to_camera = sp::Transforms::gl_world_to_camera(extrinsics);

  auto projection = sp::Transforms::gl_projection(fov, aspect_ratio, 0.01, 100);
  return sp::Camera(world_to_camera, projection);
}

std::vector<sp::Camera> load_cameras()
{
  std::ifstream input("cameras.json");
  auto cameras = sp::JsonValue::parse(input);
  return {
    load_camera(cameras["cam0"]),
    load_camera(cameras["cam1"]),
    load_camera(cameras["cam2"])};
}

void multiview()
{
  std::cout << "== Multiview ==" << std::endl;

  // One common and useful scenario for ScenePic is to visualize the result of
  // multiview 3D reconstruction. In this tutorial we'll show how to load some
  // geometry, assocaited camera calibration information, and images to create a
  // visualization depicting the results.

  sp::Scene scene;

  // load the fitted cameras
  auto cameras = load_cameras();

  // this textured cube will stand in for a reconstructed mesh
  auto texture = scene.create_image("texture");
  texture->load("PolarBear.png");

  auto cube = scene.create_mesh("cube");
  cube->texture_id(texture->image_id());
  cube->add_cube(sp::Color::None(), sp::Transforms::scale(2));

  // construct all of the frustums
  // and camera images
  auto frustums = scene.create_mesh("frustums", "frustums");
  std::vector<sp::Color> colors = {
    sp::Colors::Red, sp::Colors::Green, sp::Colors::Blue};
  std::vector<std::string> paths = {
    "render0.png", "render1.png", "render2.png"};
  std::vector<std::string> camera_images;
  std::vector<std::string> images;

  for (int i = 0; i < 3; ++i)
  {
    auto image = scene.create_image(paths[i]);
    image->load(paths[i]);
    frustums->add_camera_frustum(cameras[i], colors[i], 0.02, i + 1);
    auto image_mesh = scene.create_mesh("image" + std::to_string(i), "images");
    image_mesh->texture_id(image->image_id())
      .shared_color(sp::Colors::Gray)
      .double_sided(true);
    image_mesh->add_camera_image(cameras[i], i + 1);

    images.push_back(image->image_id());
    camera_images.push_back(image_mesh->mesh_id());
  }

  // create one canvas for each camera to show the scene from
  // that camera's viewpoint
  float width = 640;
  for (int i = 0; i < 3; ++i)
  {
    float height = width / cameras[i].aspect_ratio();
    auto canvas = scene.create_canvas_3d(
      "hand" + std::to_string(i), width, height, "", cameras[i]);

    auto frame = canvas->create_frame();
    frame->add_mesh(cube);
    frame->add_mesh(frustums);
    frame->camera(cameras[i]);
    for (int j = 0; j < 3; ++j)
    {
      frame->add_mesh_by_id(camera_images[j]);
    }
  }

  scene.save_as_html("multiview.html", "Multiview");
}

void fading()
{
  std::cout << "== Fading ==" << std::endl;

  // It is possible to use the per-frame layer settings to automatically
  // change various layer properties, for example to fade meshes in and
  // out of view. The user can still override this manually using the
  // controls, of course, but this feature can help guide the user through
  // more complex animations.

  sp::Scene scene;

  // In this tutorial we will fade out one mesh (the cube) and fade
  // another in (the sphere).

  auto cube = scene.create_mesh("cube", "cube");
  cube->add_cube(sp::Colors::Red);

  auto sphere = scene.create_mesh("sphere", "sphere");
  sphere->add_sphere(sp::Colors::Green);

  auto canvas = scene.create_canvas_3d();

  for (int i = 0; i < 60; ++i)
  {
    double sphere_opacity = i / 59.0;
    double cube_opacity = 1.0 - sphere_opacity;
    auto frame = canvas->create_frame();
    frame->add_mesh(cube);
    frame->add_mesh(sphere);
    // the interface here is the same as with how layer settings
    // usually works at the canvas level.
    frame->set_layer_settings(
      {{"cube", sp::LayerSettings(true, false, cube_opacity)},
       {"sphere", sp::LayerSettings(true, false, sphere_opacity)}});
  }

  scene.save_as_html("fading.html", "Fading");
}

int main(int argc, char* argv[])
{
  scene_and_canvas_basics();
  meshes_and_frames();
  point_clouds_1();
  point_clouds_2();
  misc_meshes();
  images_and_textures();
  canvas_2d();
  opacity_and_labels();
  animation0();
  animation1();
  camera_movement();
  audio_tracks();
  circles_video();
  multiview();
  fading();
}