
#define _USE_MATH_DEFINES

#include "scenepic.h"

#include <Eigen/Core>
#include <algorithm>
#include <cmath>
#include <tuple>

namespace sp = scenepic;

std::tuple<sp::Vector, sp::Vector> get_bounds(const sp::VectorBuffer& verts)
{
  sp::Vector vert_max = verts.colwise().maxCoeff();
  sp::Vector vert_min = verts.colwise().minCoeff();
  return std::make_tuple(0.5 * (vert_min + vert_max), vert_max - vert_min);
}

/**
 *  This class will create the animation during its initialization.
 *  This is one of many potential patterns to use when constructing more complex
 *  visualizations. The main advantage is that you can pass all configuration
 *  information into the constructor, and then share variables between
 *  different stages of the visualization or animation. This animation
 *  has three stages:
 *
 *  1. Point cloud converging to four cubes
 *  2. Four cubes morphing into the ScenePic logo
 *  3. Still shot on the logo
 *
 *  Each is constructed in a separate method, but using shared elements.
 *  One element used in this particular animation is a pre-computed camera
 *  track. While the user can still move the camera around freely,
 *  this can be helpful for create videos from scenepic (e.g. the animated
 *  GIF in the README.)
 */
class Animation
{
public:
  /** Constructor.
   *
   *  \param scene The ScenePic scene.
   *  \param num_cloud_frames Length of cloud stage
   *  \param num_morph_frames Length of morph stage
   *  \param num_still_frames Length of still stage
   *  \param start_distance Starting camera distance
   *  \param end_distance Ending camera distance.
   *  \param start_angles Starting camera angles.
   *  \param end_angles: Ending camera angles.
   *  \param width: Width of the viewport.
   *  \param height Height of the viewport.
   *  \param num_points Size of the point cloud.
   */
  Animation(
    int num_cloud_frames = 30,
    int num_morph_frames = 60,
    int num_still_frames = 30,
    float start_distance = 0.1,
    float end_distance = 2,
    const sp::Vector& start_angles = {1.5, 1, -0.1},
    const sp::Vector& end_angles = {-0.8, -0.2, 0},
    int width = 1280,
    int height = 640,
    int num_points = 1000)
  : m_num_cloud_frames(num_cloud_frames),
    m_num_morph_frames(num_morph_frames),
    m_num_still_frames(num_still_frames),
    m_num_frames(num_cloud_frames + num_morph_frames + num_still_frames),
    m_aspect_ratio(static_cast<float>(width) / static_cast<float>(height))
  {
    m_colors = {
      sp::Color::from_bytes(242, 80, 34),
      sp::Color::from_bytes(127, 186, 0),
      sp::Color::from_bytes(0, 164, 239),
      sp::Color::from_bytes(255, 185, 0)};

    m_canvas = m_scene.create_canvas_3d("main", width, height);

    // load the text meshes
    auto scene_obj = sp::load_obj("scene.obj");
    m_scene_mesh = m_scene.create_mesh("scene_text", "text");
    m_scene_mesh->shared_color(m_colors[0]);
    m_scene_mesh->add_mesh(scene_obj);
    m_scene_mesh->apply_transform(
      sp::Transforms::rotation_about_x(static_cast<float>(M_PI / 2)));
    m_scene_pos = {0, 0, 0};

    auto pic_obj = sp::load_obj("pic.obj");
    m_pic_mesh = m_scene.create_mesh("pic_text", "text");
    m_pic_mesh->shared_color(m_colors[3]);
    m_pic_mesh->add_mesh(pic_obj);
    m_pic_pos = {1.25, -0.05, 0.5};

    // create the cubes for morphing
    std::transform(
      m_colors.begin(),
      m_colors.end(),
      std::back_inserter(m_cubes),
      [this](const sp::Color& color) { return this->create_cube(color); });

    m_cube1_pos = {2, 0.15, 0.25};
    m_cube2_pos = {0.4, -0.05, 0.3};
    m_cube2_scale = {1.5, 0.2, 0.8};
    m_focus_point = {1, 0.25, 0.5};

    // compute the angles and distances for the cameras
    // the camera is parametrized as a "look-at" camera,
    // with a Z-distance from the focus point that is rotated
    // using the provided angles to get a center, and
    // then focused on a set point.
    std::tie(m_angles, m_distances) = compute_camera_info(
      start_distance, end_distance, start_angles, end_angles);

    m_index = 0;
    animate_cloud(num_points);
    animate_morph();
    animate_still();
  }

  /** Save the animation to the specified path.
   *
   *  \param path path to the HTML output file
   */
  void save(const std::string& path)
  {
    m_scene.save_as_html(path, "Hello Scenepic!");
  }

private:
  std::tuple<sp::VectorBuffer, sp::ValueBuffer> compute_camera_info(
    float start_distance,
    float end_distance,
    const sp::Vector& start_angles,
    const sp::Vector& end_angles)
  {
    sp::VectorBuffer angles(m_num_frames, 3);
    angles.topRows(m_num_cloud_frames) = sp::linspace<sp::VectorBuffer>(
      m_num_cloud_frames, start_angles, sp::Vector(0, 0, 0));
    angles.middleRows(m_num_cloud_frames, m_num_morph_frames) =
      sp::linspace<sp::VectorBuffer>(
        m_num_morph_frames, sp::Vector(0, 0, 0), end_angles);
    angles.bottomRows(m_num_still_frames) =
      end_angles.colwise().replicate(m_num_still_frames);

    sp::ValueBuffer distances(m_num_frames);
    distances.fill(end_distance);
    distances.topRows(m_num_cloud_frames)
      .setLinSpaced(start_distance, end_distance);

    return std::make_tuple(angles, distances);
  }

  sp::Camera create_camera(const sp::Vector& focus_point)
  {
    // compute the rotation matrix
    auto rotation =
      sp::Transforms::euler_angles_to_matrix(m_angles.row(m_index));

    // create a vector at the current distance
    sp::Vector pos(0, 0, m_distances(m_index));

    // rotate and offset from the focus point
    pos = rotation.topLeftCorner(3, 3) * pos.transpose();
    pos += focus_point;

    // NB each call to this function will move the camera long its track
    m_index += 1;

    sp::Camera camera(pos, focus_point);
    camera.aspect_ratio(m_aspect_ratio);
    return camera;
  }

  void animate_cloud(int num_points)
  {
    const int num_frames = m_num_cloud_frames;

    // start with random positions
    sp::VectorBuffer start_positions =
      sp::random<sp::VectorBuffer>(num_points, 3, -1, 1);
    sp::VectorBuffer end_positions = sp::VectorBuffer::Zero(num_points, 3);

    // group the points by which will go to each cube
    int group_size = num_points / 4;
    int final_group_size = num_points - (3 * group_size);

    // distribute the final points randomly within each target cube
    end_positions.topRows(group_size) = sp::random<sp::VectorBuffer>(
      group_size,
      3,
      sp::Vector(-0.51, 0.03, -0.22),
      sp::Vector(-0.03, 0.51, 0.22));
    end_positions.middleRows(group_size, group_size) =
      sp::random<sp::VectorBuffer>(
        group_size,
        3,
        sp::Vector(0.03, 0.03, -0.22),
        sp::Vector(0.51, 0.51, 0.22));
    end_positions.middleRows(2 * group_size, group_size) =
      sp::random<sp::VectorBuffer>(
        group_size,
        3,
        sp::Vector(-0.51, -0.51, -0.22),
        sp::Vector(-0.03, -0.03, 0.22));
    end_positions.bottomRows(final_group_size) = sp::random<sp::VectorBuffer>(
      final_group_size,
      3,
      sp::Vector(0.03, -0.51, -0.22),
      sp::Vector(0.51, -0.03, 0.22));

    // set the cloud colors to correspond to the target cubes
    sp::ColorBuffer colors(num_points, 3);
    colors.topRows(group_size) =
      m_colors[0].rowwise().replicate(group_size).transpose();
    colors.middleRows(group_size, group_size) =
      m_colors[1].rowwise().replicate(group_size).transpose();
    colors.middleRows(2 * group_size, group_size) =
      m_colors[2].rowwise().replicate(group_size).transpose();
    colors.bottomRows(final_group_size) =
      m_colors[3].rowwise().replicate(final_group_size).transpose();

    sp::VectorBuffer pos_diff = (end_positions - start_positions) / num_frames;
    sp::VectorBuffer positions = start_positions;

    for (int i = 0; i < num_frames; ++i, positions += pos_diff)
    {
      auto mesh = m_scene.create_mesh("", "cloud");
      mesh->add_cube(sp::Colors::White, sp::Transforms::scale(0.05));
      mesh->enable_instancing(positions, sp::QuaternionBufferNone(), colors);

      auto frame = m_canvas->create_frame();
      frame->add_mesh(mesh);
      frame->camera(create_camera({0, 0, 0}));
    }
  }

  void animate_morph()
  {
    const int num_frames = m_num_morph_frames;

    // because the text will be hiding within the cubes
    // and then eventually revealed, we need to compute
    // some bounds information to position it correctly

    sp::Vector scene_pos, scene_scale;
    std::tie(scene_pos, scene_scale) =
      get_bounds(m_scene_mesh->vertex_positions());
    sp::VectorBuffer scene_positions(num_frames, 3);
    sp::linspace(scene_positions, sp::Vector(-0.47, 0.07, 0), m_scene_pos);
    sp::VectorBuffer scene_scales(num_frames, 3);
    sp::Vector start_scale = 0.4 * scene_scale.cwiseInverse();
    sp::linspace(scene_scales, start_scale, sp::Vector(1, 1, 1));
    scene_scales.col(2).array() = 1;

    sp::Vector pic_pos, pic_scale;
    std::tie(pic_pos, pic_scale) = get_bounds(m_pic_mesh->vertex_positions());
    sp::VectorBuffer pic_positions = sp::linspace<sp::VectorBuffer>(
      num_frames, sp::Vector(0.27, -0.27, pic_scale.z() * 0.5), m_pic_pos);
    start_scale = 0.4 * pic_scale.cwiseInverse();
    sp::VectorBuffer pic_scales = sp::linspace<sp::VectorBuffer>(
      num_frames, start_scale, sp::Vector(1, 1, 1));
    pic_pos = m_pic_pos;
    pic_pos.z() = pic_pos.z() - 0.5 * pic_scale.z();

    // create the animated positions and scales for the cubes
    std::vector<sp::VectorBuffer> positions = {
      sp::linspace<sp::VectorBuffer>(
        num_frames, sp::Vector(-0.27, 0.27, 0), scene_pos),
      sp::linspace<sp::VectorBuffer>(
        num_frames, sp::Vector(0.27, 0.27, 0), m_cube1_pos),
      sp::linspace<sp::VectorBuffer>(
        num_frames, sp::Vector(-0.27, -0.27, 0), m_cube2_pos),
      sp::linspace<sp::VectorBuffer>(
        num_frames, sp::Vector(0.27, -0.27, 0), pic_pos)};

    scene_scale *= 2;
    pic_scale *= 2;
    std::vector<sp::VectorBuffer> scales = {
      sp::linspace<sp::VectorBuffer>(
        num_frames, sp::Vector(1, 1, 1), scene_scale),
      sp::linspace<sp::VectorBuffer>(
        num_frames, sp::Vector(1, 1, 1), sp::Vector(1, 1, 1)),
      sp::linspace<sp::VectorBuffer>(
        num_frames, sp::Vector(1, 1, 1), m_cube2_scale),
      sp::linspace<sp::VectorBuffer>(
        num_frames, sp::Vector(1, 1, 1), pic_scale)};

    scales[0].col(2).setLinSpaced(1.0f, 0.0f);
    scales[3].col(1).setLinSpaced(1.0f, 0.0f);

    sp::VectorBuffer focus_points = sp::linspace<sp::VectorBuffer>(
      num_frames, sp::Vector(0, 0, 0), m_focus_point);

    for (int f = 0; f < num_frames; ++f)
    {
      auto frame = m_canvas->create_frame();

      sp::Transform transform;
      for (int i = 0; i < 4; ++i)
      {
        transform = sp::Transforms::translate(positions[i].row(f)) *
          sp::Transforms::scale(scales[i].row(f));
        frame->add_mesh(m_cubes[i], transform);
      }

      transform = sp::Transforms::translate(scene_positions.row(f)) *
        sp::Transforms::scale(scene_scales.row(f));
      frame->add_mesh(m_scene_mesh, transform);

      transform = sp::Transforms::translate(pic_positions.row(f)) *
        sp::Transforms::scale(pic_scales.row(f));
      frame->add_mesh(m_pic_mesh, transform);
      frame->camera(create_camera(focus_points.row(f)));
      frame->focus_point(sp::FocusPoint(focus_points.row(f)));
    }
  }

  std::shared_ptr<sp::Mesh> create_cube(const sp::Color& color)
  {
    auto cube = m_scene.create_mesh("", "cubes");
    cube->shared_color(color);
    cube->add_cube(sp::Color::None(), sp::Transforms::scale(0.5));
    return cube;
  }

  void animate_still()
  {
    // this creates the ScenePic logo
    std::vector<std::shared_ptr<sp::Mesh>> meshes = {
      m_scene_mesh, m_cubes[1], m_cubes[2], m_pic_mesh};
    std::vector<sp::Transform> transforms = {
      sp::Transforms::translate(m_scene_pos),
      sp::Transforms::translate(m_cube1_pos),
      sp::Transforms::translate(m_cube2_pos) *
        sp::Transforms::scale(m_cube2_scale),
      sp::Transforms::translate(m_pic_pos)};

    for (int f = 0; f < m_num_still_frames; ++f)
    {
      auto frame = m_canvas->create_frame();

      for (int i = 0; i < 4; ++i)
      {
        frame->add_mesh(meshes[i], transforms[i]);
      }

      frame->camera(create_camera(m_focus_point));
      frame->focus_point(sp::FocusPoint(m_focus_point));
    }
  }

  int m_num_cloud_frames;
  int m_num_morph_frames;
  int m_num_still_frames;
  int m_num_frames;
  float m_aspect_ratio;
  std::vector<sp::Color> m_colors;
  sp::Scene m_scene;
  std::shared_ptr<sp::Canvas3D> m_canvas;
  std::shared_ptr<sp::Mesh> m_scene_mesh;
  sp::Vector m_scene_pos;
  std::shared_ptr<sp::Mesh> m_pic_mesh;
  sp::Vector m_pic_pos;
  std::vector<std::shared_ptr<sp::Mesh>> m_cubes;
  sp::Vector m_cube1_pos;
  sp::Vector m_cube2_pos;
  sp::Vector m_cube2_scale;
  sp::Vector m_focus_point;
  sp::VectorBuffer m_angles;
  sp::ValueBuffer m_distances;
  int m_index;
};

int main(int argc, char* argv[])
{
  Animation anim;
  anim.save("hello_scenepic.html");
}
