// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "scene.h"

#include "internal.h"
#include "js_lib.h"
#include "util.h"

#include "json/json.h"
#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

namespace scenepic
{
  Scene::Scene(const std::string& scene_id)
  : m_scene_id(scene_id),
    m_num_canvases(0),
    m_num_meshes(0),
    m_num_images(0),
    m_num_audios(0),
    m_num_videos(0),
    m_num_labels(0),
    m_num_text_panels(0),
    m_num_drop_down_menus(0),
    m_script_cleared(false),
    m_fps(30.0f),
    m_status_bar_visibility("visible")
  {}

  std::shared_ptr<Canvas3D> Scene::create_canvas_3d(
    const std::string& canvas_id_init,
    double width,
    double height,
    const std::string& html_id,
    const Camera& camera_init,
    const Shading& shading,
    const UIParameters& ui_parameters,
    const std::string& media_id)
  {
    std::string canvas_id = canvas_id_init;
    if (canvas_id.empty())
    {
      canvas_id = "Canvas-" + std::to_string(m_num_canvases);
    }

    Camera camera;
    if (camera_init.is_none())
    {
      camera.aspect_ratio(width / height);
    }
    else
    {
      camera = camera_init;
    }

    auto canvas =
      std::make_shared<Canvas3D>(Canvas3D(canvas_id, width, height));
    canvas->camera(camera);
    canvas->shading(shading);
    canvas->ui_parameters(ui_parameters);
    canvas->media_id(media_id);
    m_canvas3Ds.push_back(canvas);
    m_num_canvases += 1;

    JsonValue display_obj;
    display_obj["CommandType"] = "AddCanvas3D";
    display_obj["CanvasId"] = canvas_id;

    if (width == std::floor(width))
    {
      display_obj["Width"] = static_cast<std::int64_t>(width);
    }
    else
    {
      display_obj["Width"] = width;
    }

    if (height == std::floor(height))
    {
      display_obj["Height"] = static_cast<std::int64_t>(height);
    }
    else
    {
      display_obj["Height"] = height;
    }

    if (!html_id.empty())
    {
      display_obj["HtmlId"] = html_id;
    }

    m_display_order.push_back(display_obj);

    return canvas;
  }

  std::shared_ptr<Canvas2D> Scene::create_canvas_2d(
    const std::string& canvas_id_init,
    double width,
    double height,
    const std::string& html_id,
    const Color& background_color,
    const std::string& media_id)
  {
    std::string canvas_id = canvas_id_init;
    if (canvas_id.empty())
    {
      canvas_id = "Canvas-" + std::to_string(m_num_canvases);
    }

    auto canvas =
      std::make_shared<Canvas2D>(Canvas2D(canvas_id, width, height));
    canvas->background_color(background_color);
    canvas->media_id(media_id);
    m_canvas2Ds.push_back(canvas);
    m_num_canvases += 1;

    JsonValue display_obj;
    display_obj["CommandType"] = "AddCanvas2D";
    display_obj["CanvasId"] = canvas_id;

    if (width == std::floor(width))
    {
      display_obj["Width"] = static_cast<std::int64_t>(width);
    }
    else
    {
      display_obj["Width"] = width;
    }

    if (height == std::floor(height))
    {
      display_obj["Height"] = static_cast<std::int64_t>(height);
    }
    else
    {
      display_obj["Height"] = height;
    }

    if (!html_id.empty())
    {
      display_obj["HtmlId"] = html_id;
    }

    m_display_order.push_back(display_obj);

    return canvas;
  }

  std::shared_ptr<Graph> Scene::create_graph(
    const std::string& canvas_id_init,
    double width,
    double height,
    const std::string& name_align,
    const std::string& value_align,
    const std::string& html_id,
    const Color& background_color,
    const Graph::Margin& margin,
    const std::string& font_family,
    float name_size,
    float value_size,
    const std::string& media_id)
  {
    std::string canvas_id = canvas_id_init;
    if (canvas_id.empty())
    {
      canvas_id = "Canvas-" + std::to_string(m_num_canvases);
    }

    auto canvas = std::make_shared<Graph>(Graph(canvas_id));
    canvas->background_color(background_color);
    canvas->media_id(media_id);
    canvas->margin(margin);
    canvas->font_family(font_family);
    canvas->name_align(name_align);
    canvas->name_size(name_size);
    canvas->value_align(value_align);
    canvas->value_size(value_size);
    m_graphs.push_back(canvas);
    m_num_canvases += 1;

    JsonValue display_obj;
    display_obj["CommandType"] = "AddGraph";
    display_obj["CanvasId"] = canvas_id;

    if (width == std::floor(width))
    {
      display_obj["Width"] = static_cast<std::int64_t>(width);
    }
    else
    {
      display_obj["Width"] = width;
    }

    if (height == std::floor(height))
    {
      display_obj["Height"] = static_cast<std::int64_t>(height);
    }
    else
    {
      display_obj["Height"] = height;
    }

    if (!html_id.empty())
    {
      display_obj["HtmlId"] = html_id;
    }

    m_display_order.push_back(display_obj);

    return canvas;
  }

  std::shared_ptr<AudioTrack>
  Scene::create_audio(const std::string& audio_id_init)
  {
    std::string audio_id = audio_id_init;
    if (audio_id.empty())
    {
      audio_id = "AudioTrack-" + std::to_string(m_num_audios);
    }

    auto audio = std::make_shared<AudioTrack>(AudioTrack(audio_id));
    m_audios.push_back(audio);
    m_num_audios += 1;

    return audio;
  }

  std::shared_ptr<Video> Scene::create_video(const std::string& video_id_init)
  {
    std::string video_id = video_id_init;
    if (video_id.empty())
    {
      video_id = "Video-" + std::to_string(m_num_videos);
    }

    auto video = std::make_shared<Video>(Video(video_id));
    m_videos.push_back(video);
    m_num_videos += 1;

    return video;
  }

  std::shared_ptr<Mesh> Scene::create_mesh(
    const std::string& mesh_id_init,
    const std::string& layer_id,
    bool double_sided,
    bool camera_space,
    const Color& shared_color,
    const std::string& texture_id,
    bool nn_texture,
    bool use_texture_alpha,
    bool is_billboard,
    bool is_label)
  {
    std::string mesh_id = mesh_id_init;
    if (mesh_id.empty())
    {
      mesh_id = "Mesh-" + std::to_string(m_num_meshes);
    }

    auto mesh = std::make_shared<Mesh>(Mesh(mesh_id));
    mesh->layer_id(layer_id);
    mesh->double_sided(double_sided);
    mesh->camera_space(camera_space);
    mesh->shared_color(shared_color);
    mesh->texture_id(texture_id);
    mesh->nn_texture(nn_texture);
    mesh->use_texture_alpha(use_texture_alpha);
    mesh->is_billboard(is_billboard);
    mesh->is_label(is_label);
    m_meshes.push_back(mesh);
    m_num_meshes += 1;
    return mesh;
  }

  std::shared_ptr<MeshUpdate> Scene::update_mesh(
    const std::string& base_mesh_id,
    const ConstVectorBufferRef& positions,
    const ConstVectorBufferRef& normals,
    const ConstVectorBufferRef& colors,
    const std::string& mesh_id_init)
  {
    std::string mesh_id = mesh_id_init;
    if (mesh_id.empty())
    {
      mesh_id = "Mesh-" + std::to_string(m_num_meshes);
    }

    auto it = std::find_if(
      m_meshes.begin(), m_meshes.end(), [&](const std::shared_ptr<Mesh>& mesh) {
        return mesh->mesh_id() == base_mesh_id;
      });
    if (it == m_meshes.end())
    {
      throw std::invalid_argument("Invalid base mesh ID");
    }
    auto base_mesh = *it;

    if (m_update_counts.count(base_mesh_id) == 0)
    {
      m_update_counts[base_mesh_id] = 0;
    }

    auto frame_index = m_update_counts[base_mesh_id];
    m_update_counts[base_mesh_id] = frame_index + 1;

    std::vector<ConstVertexBufferRef> buffers = {positions, normals, colors};

    std::vector<VertexBufferType> buffer_types = {
      VertexBufferType::Positions,
      VertexBufferType::Normals,
      VertexBufferType::Colors};

    auto mesh_update = std::make_shared<MeshUpdate>(
      MeshUpdate(base_mesh_id, mesh_id, buffers, buffer_types, frame_index));
    m_mesh_updates.push_back(mesh_update);
    m_num_meshes += 1;
    return mesh_update;
  }

  std::shared_ptr<MeshUpdate> Scene::update_instanced_mesh(
    const std::string& base_mesh_id,
    const ConstVectorBufferRef& positions,
    const ConstQuaternionBufferRef& rotations,
    const ConstVectorBufferRef& colors,
    const std::string& mesh_id_init)
  {
    std::string mesh_id = mesh_id_init;
    if (mesh_id.empty())
    {
      mesh_id = "Mesh-" + std::to_string(m_num_meshes);
    }

    auto it = std::find_if(
      m_meshes.begin(), m_meshes.end(), [&](const std::shared_ptr<Mesh>& mesh) {
        return mesh->mesh_id() == base_mesh_id;
      });
    if (it == m_meshes.end())
    {
      throw std::invalid_argument("Invalid base mesh ID");
    }
    auto base_mesh = *it;

    if (m_update_counts.count(base_mesh_id) == 0)
    {
      m_update_counts[base_mesh_id] = 0;
    }

    auto frame_index = m_update_counts[base_mesh_id];
    m_update_counts[base_mesh_id] = frame_index + 1;

    std::vector<ConstVertexBufferRef> buffers = {positions, rotations, colors};

    std::vector<VertexBufferType> buffer_types = {
      VertexBufferType::Positions,
      VertexBufferType::Rotations,
      VertexBufferType::Colors};

    auto mesh_update = std::make_shared<MeshUpdate>(
      MeshUpdate(base_mesh_id, mesh_id, buffers, buffer_types, frame_index));
    m_mesh_updates.push_back(mesh_update);
    m_num_meshes += 1;
    return mesh_update;
  }

  std::shared_ptr<MeshUpdate> Scene::update_mesh_without_normals(
    const std::string& base_mesh_id,
    const ConstVectorBufferRef& positions,
    const std::string& mesh_id_init)
  {
    return update_mesh_positions(base_mesh_id, positions, mesh_id_init);
  }

  std::shared_ptr<MeshUpdate> Scene::update_mesh_positions(
    const std::string& base_mesh_id,
    const ConstVectorBufferRef& positions,
    const std::string& mesh_id_init)
  {
    auto it = std::find_if(
      m_meshes.begin(), m_meshes.end(), [&](const std::shared_ptr<Mesh>& mesh) {
        return mesh->mesh_id() == base_mesh_id;
      });
    if (it == m_meshes.end())
    {
      throw std::invalid_argument("Invalid base mesh ID");
    }
    auto base_mesh = *it;

    if (base_mesh->is_instanced())
    {
      return update_instanced_mesh(
        base_mesh_id,
        positions,
        QuaternionBufferNone(),
        ColorBufferNone(),
        mesh_id_init);
    }
    else
    {
      return update_mesh(
        base_mesh_id,
        positions,
        VectorBufferNone(),
        ColorBufferNone(),
        mesh_id_init);
    }
  }

  std::shared_ptr<Image> Scene::create_image(const std::string& image_id_init)
  {
    std::string image_id = image_id_init;
    if (image_id.empty())
    {
      image_id = "Image-" + std::to_string(m_num_images);
    }

    auto image = std::make_shared<Image>(Image(image_id));
    m_images.push_back(image);
    m_num_images += 1;
    return image;
  }

  std::shared_ptr<Label> Scene::create_label(
    const std::string& label_id_init,
    const std::string& text,
    const Color& color,
    double size_in_pixels,
    const std::string& font_family,
    const std::string& layer_id,
    const std::string& horizontal_align,
    const std::string& vertical_align,
    double offset_distance,
    bool camera_space)
  {
    std::string label_id = label_id_init;
    if (label_id.empty())
    {
      label_id = "Label-" + std::to_string(m_num_labels);
    }

    auto mesh = this->create_mesh();
    mesh->double_sided(false)
      .shared_color(color)
      .texture_id(label_id)
      .nn_texture(false)
      .is_label(true)
      .layer_id(layer_id)
      .camera_space(camera_space);
    mesh->add_image(
      Vector(-0.5f, -0.5f, 0),
      Vector(1, 0, 0),
      Vector(0, 1, 0),
      VectorNone(),
      UV(0, 0),
      UV(1, 0),
      UV(1, 1),
      UV(0, 1),
      false);

    auto label = std::make_shared<Label>(Label(label_id, mesh));
    label->text(text);
    label->fill_color(color);
    label->size_in_pixels(size_in_pixels);
    label->font_family(font_family);
    label->horizontal_align(horizontal_align);
    label->vertical_align(vertical_align);
    label->offset_distance(offset_distance);
    m_labels.push_back(label);
    m_num_labels += 1;

    return label;
  }

  std::shared_ptr<TextPanel> Scene::create_text_panel(
    const std::string& text_panel_id_init,
    const std::string& html_id,
    bool add_input_box)
  {
    std::string text_panel_id = text_panel_id_init;
    if (text_panel_id.empty())
    {
      text_panel_id = "TextPanel-" + std::to_string(m_num_text_panels);
    }

    auto text_panel = std::make_shared<TextPanel>(TextPanel(text_panel_id));
    m_text_panels.push_back(text_panel);
    m_num_text_panels += 1;

    JsonValue display_obj;
    display_obj["CommandType"] = "AddTextPanel";
    display_obj["TextPanelId"] = text_panel_id;
    display_obj["AddInputBox"] = add_input_box;
    if (!html_id.empty())
    {
      display_obj["HtmlId"] = html_id;
    }

    m_display_order.push_back(display_obj);
    return text_panel;
  }

  std::shared_ptr<DropDownMenu> Scene::create_drop_down_menu(
    const std::string& drop_down_menu_id_init,
    const std::string& title,
    const std::string& html_id)
  {
    std::string drop_down_menu_id = drop_down_menu_id_init;
    if (drop_down_menu_id.empty())
    {
      drop_down_menu_id =
        "DropDownMenu-" + std::to_string(m_num_drop_down_menus);
    }

    auto drop_down_menu =
      std::make_shared<DropDownMenu>(DropDownMenu(drop_down_menu_id));
    m_drop_down_menus.push_back(drop_down_menu);
    m_num_drop_down_menus += 1;

    JsonValue display_obj;
    display_obj["CommandType"] = "AddDropDownMenu";
    display_obj["DropDownMenuId"] = drop_down_menu_id;
    display_obj["Title"] = title;
    if (!html_id.empty())
    {
      display_obj["HtmlId"] = html_id;
    }

    m_display_order.push_back(display_obj);
    return drop_down_menu;
  }

  void Scene::configure_user_interface(const UIParameters& ui_parameters)
  {
    m_misc.push_back(ui_parameters.to_json());
  }

  void Scene::link_canvas_events(const std::vector<std::string>& canvas_ids)
  {
    JsonValue command;
    command["CommandType"] = "LinkCanvasEvents";
    for (const auto& id : canvas_ids)
    {
      command["CanvasIds"].append(id);
    }
    m_misc.push_back(command);
  }

  void Scene::simulate_key_presses(
    const std::string& canvas_id, const std::vector<std::string>& keypresses)
  {
    for (const auto& keypress : keypresses)
    {
      JsonValue command;
      command["CommandType"] = "SimulateKeyPress";
      command["CanvasId"] = canvas_id;
      command["Key"] = keypress;
      m_misc.push_back(command);
    }
  }

  void _measure_commands(
    std::map<std::string, std::size_t>& command_sizes, Json::Value commands)
  {
    for (auto& command : commands)
    {
      if (command.type() == Json::objectValue && command.isMember("Commands"))
      {
        _measure_commands(command_sizes, command["Commands"]);
      }
      else if (command.type() == Json::arrayValue)
      {
        _measure_commands(command_sizes, command);
      }
      else
      {
        std::size_t size = command.toStyledString().length();
        std::string key = command["CommandType"].asString();
        command_sizes[key] += size;
      }
    }
  }

  void measure_commands(
    std::map<std::string, std::size_t>& command_sizes, JsonValue commands)
  {
    _measure_commands(command_sizes, scenepic_to_json(commands));
  }

  std::map<std::string, std::size_t> Scene::measure_command_size() const
  {
    JsonValue commands = this->to_json();
    std::map<std::string, std::size_t> command_sizes;
    measure_commands(command_sizes, commands);
    return command_sizes;
  }

  JsonValue Scene::to_json() const
  {
    JsonValue commands;
    commands.resize(0);
    if (!m_scene_id.empty())
    {
      JsonValue command;
      command["CommandType"] = "SetSceneId";
      command["SceneId"] = m_scene_id;
      commands.append(command);
    }

    JsonValue properties;
    properties["CommandType"] = "SetSceneProperties";
    properties["FrameRate"] = m_fps;
    properties["StatusBarVisibility"] = m_status_bar_visibility;
    commands.append(properties);

    Scene::add_commands(commands, m_meshes);

    // add key frames first
    for (auto& update : m_mesh_updates)
    {
      if (!update->is_quantized())
      {
        commands.append(update->to_json());
      }
    }

    // then add quantized frames
    for (auto& update : m_mesh_updates)
    {
      if (update->is_quantized())
      {
        commands.append(update->to_json());
      }
    }

    Scene::add_commands(commands, m_images);
    Scene::add_commands(commands, m_videos);
    Scene::add_commands(commands, m_audios);
    Scene::add_commands(commands, m_labels);

    for (const auto& display_obj : m_display_order)
    {
      commands.append(display_obj);
    }

    Scene::add_commands(commands, m_canvas2Ds);
    Scene::add_commands(commands, m_canvas3Ds);
    Scene::add_commands(commands, m_graphs);
    Scene::add_commands(commands, m_text_panels);
    Scene::add_commands(commands, m_drop_down_menus);

    for (const auto& misc : m_misc)
    {
      commands.append(misc);
    }

    return commands;
  }

  void Scene::clear_script()
  {
    m_scene_id = "";
    m_meshes.clear();
    m_mesh_updates.clear();
    m_images.clear();
    m_audios.clear();
    m_labels.clear();
    m_display_order.clear();
    for (auto& canvas : m_canvas2Ds)
    {
      canvas->clear_script();
    }

    for (auto& canvas : m_canvas3Ds)
    {
      canvas->clear_script();
    }

    for (auto& text_panel : m_text_panels)
    {
      text_panel->clear_script();
    }

    for (auto& drop_down_menu : m_drop_down_menus)
    {
      drop_down_menu->clear_script();
    }

    m_misc.clear();
    m_script_cleared = true;
  }

  float Scene::framerate() const
  {
    return m_fps;
  }

  void Scene::framerate(float fps)
  {
    m_fps = fps;
  }

  const std::string& Scene::status_bar_visibility() const
  {
    return m_status_bar_visibility;
  }

  void Scene::status_bar_visibility(const std::string& visibility)
  {
    m_status_bar_visibility = visibility;
  }

  std::string Scene::json() const
  {
    return this->to_json().to_string();
  }

  std::string Scene::script() const
  {
    std::stringstream buff;
    buff << "window.onload = function(){\n"
         << "    let commands = " << this->json() << ";\n"
         << "    scenepic(null, commands);\n"
         << "}\n";

    return buff.str();
  }

  void Scene::save_as_json(const std::string& path) const
  {
    std::ofstream output(path);
    output << this->json();
  }

  void Scene::save_as_script(const std::string& path, bool standalone) const
  {
    std::ofstream output(path);
    if (standalone)
    {
      std::stringstream buff;
      for (auto& line : JS_LIB_SRC)
      {
        buff << line;
      }

      output << buff.str() << std::endl << std::endl;
    }

    output << this->script();
  }

  void Scene::save_as_html(
    const std::string& path,
    const std::string& title,
    const std::string& head_html,
    const std::string& body_html,
    const std::string& script_path,
    const std::string& library_path)
  {
    if (m_script_cleared)
    {
      throw std::logic_error(
        "You should not call clear_script() on Scenes that you wish to "
        "save_as_html().");
    }

    std::string script = this->script();
    std::string path_to_script = "";
    if (!script_path.empty())
    {
      std::ofstream script_file(script_path);
      script_file << script;
      script = "";
      path_to_script = " src='" + script_path + "'";
    }

    std::string lib = "";
    std::string path_to_lib = "";
    std::stringstream buff;
    for (auto& line : JS_LIB_SRC)
    {
      buff << line;
    }
    lib = buff.str();

    if (!library_path.empty())
    {
      std::ofstream lib_file(library_path);
      lib_file << lib;
      lib = "";
      path_to_lib = " src='" + library_path + "'";
    }

    std::ofstream html(path);
    html << "<!DOCTYPE html>" << std::endl
         << "<html lang=\"en\">" << std::endl
         << "   <head>" << std::endl
         << "      <meta charset=\"utf-8\"/>" << std::endl
         << "      <title>" << title << "</title>" << std::endl
         << "      <script" << path_to_lib << ">" << lib << "</script>"
         << std::endl
         << "      <script" << path_to_script << ">" << script << "</script>"
         << std::endl
         << "      " << head_html << std::endl
         << "   </head>" << std::endl
         << "   <body>" << std::endl
         << body_html << std::endl
         << "   </body>" << std::endl
         << "</html>";
  }

  bool Scene::script_cleared() const
  {
    return m_script_cleared;
  }

  void Scene::grid(
    const std::string& width,
    const std::string& grid_template_rows,
    const std::string& grid_template_columns)
  {
    JsonValue display_obj;
    display_obj["CommandType"] = "DefineGrid";
    display_obj["Width"] = width;
    display_obj["GridTemplateRows"] = grid_template_rows;
    display_obj["GridTemplateColumns"] = grid_template_columns;
    m_display_order.push_back(display_obj);
  }

  void Scene::place(
    const std::string& canvas_id,
    const std::string& grid_row,
    const std::string& grid_column)
  {
    JsonValue display_obj;
    display_obj["CommandType"] = "PlaceCanvasInGrid";
    display_obj["CanvasId"] = canvas_id;
    display_obj["GridRow"] = grid_row;
    display_obj["GridColumn"] = grid_column;
    m_display_order.push_back(display_obj);
  }
} // namespace scenepic