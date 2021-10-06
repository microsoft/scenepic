#include "scenepic.h"

#include <vector>

namespace sp = scenepic;

int main(int argc, char* argv[])
{
  auto scene_obj = sp::load_obj("scene.obj");
  auto pic_obj = sp::load_obj("pic.obj");

  std::vector<sp::Color> colors = {
    sp::Color::from_bytes(242, 80, 34),
    sp::Color::from_bytes(127, 186, 0),
    sp::Color::from_bytes(0, 164, 239),
    sp::Color::from_bytes(255, 185, 0)};

  sp::Scene scene;
  auto canvas = scene.create_canvas_3d("main", 1280, 640);

  auto scene_mesh = scene.create_mesh();
  scene_mesh->shared_color(colors[0]);
  scene_mesh->add_mesh(scene_obj);

  auto pic_mesh = scene.create_mesh();
  pic_mesh->shared_color(colors[1]);
  pic_mesh->add_mesh(pic_obj);

  auto frame = canvas->create_frame();
  frame->add_mesh(scene_mesh);

  frame = canvas->create_frame();
  frame->add_mesh(pic_mesh);

  scene.save_as_html("hello_scenepic.html", "Hello Scenepic!");
}