// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_SCENE_H_
#define _SCENEPIC_SCENE_H_

#include "audio_track.h"
#include "camera.h"
#include "canvas2d.h"
#include "canvas3d.h"
#include "color.h"
#include "drop_down_menu.h"
#include "graph.h"
#include "image.h"
#include "mesh.h"
#include "mesh_update.h"
#include "shading.h"
#include "text_panel.h"
#include "ui_parameters.h"
#include "video.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace scenepic
{
  /** Information about the results of quantization. */
  struct QuantizationInfo
  {
    QuantizationInfo() = default;

    /** Constructor. */
    QuantizationInfo(
      std::uint32_t keyframe_count,
      float estimated_size_ratio,
      float mean_error,
      float max_error);

    /** The number of keyframes used */
    std::uint32_t keyframe_count;

    /** The estimated size ratio after compression. */
    float estimated_size_ratio;

    /** The mean per-frame error */
    float mean_error;

    /** The maximum per-frame error */
    float max_error;

    std::string to_string() const;
  };

  /** Top level container representing an entire ScenePic. */
  class Scene
  {
  public:
    /** Constructor.
     *  \param scene_id a unique identifier for the Scene (will be automatically
     *                  populated if not provided).
     */
    Scene(const std::string& scene_id = "");

    /** Defines a CSS layout grid for the canvases of the ScenePic. Should not
     *  be used in conjunction with `html_id` for individual canvases. See
     *  https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Grid_Layout
     *  for more information.
     *  \param width CSS declaration for the width of the ScenePic.
     *  \param grid_template_rows CSS declaration defining the rows of the
     *                            ScenePic
     *  \param grid_template_cols CSS declaration defining the columns of the
     *                            ScenePic
     */
    void grid(
      const std::string& width,
      const std::string& grid_template_rows = "",
      const std::string& grid_template_cols = "");

    /** Places a canvas at a specific place in the ScenePic grid.
     *  @see scenepic::Scene::grid.
     *  \param canvas_id the unique ID of the Canvas to place.
     *  \param grid_row the CSS row specification for the Canvas
     *  \param grid_column the CSS column specification for the Canvas
     */
    void place(
      const std::string& canvas_id,
      const std::string& grid_row,
      const std::string& grid_column);

    /** Create a new 3D Canvas and append to the Scene.
     *  \param canvas_id a unique identifier for the Canvas (will be
     *                   automatically populated if not provided).
     *  \param width the width in pixels of the Canvas on the HTML page
     *  \param height the height in pixels of the Canvas on the HTML page
     *  \param html_id optional id of an HTML element to use as this Canvas's
     *                 parent in the HTML DOM (otherwise simply appended to
     *                 document).
     *  \param camera optional set of Camera parameters for the canvas
     *  \param shading optional set of shading parameters for the canvas
     *  \param ui_parameters optional set of user interface parameters
     *  \param media_id optional ID of a media file to attach to the canvas.
     *                  This file will be used to drive playback, i.e. frames
     *                  will be displayed in time with the playback of the
     *                  media file.
     *  \return a reference to the new Canvas object
     */
    std::shared_ptr<Canvas3D> create_canvas_3d(
      const std::string& canvas_id = "",
      double width = 400,
      double height = 400,
      const std::string& html_id = "",
      const Camera& camera = Camera::None(),
      const Shading& shading = Shading(),
      const UIParameters& ui_parameters = UIParameters(),
      const std::string& media_id = "");

    /** Create a new 2D Canvas and append to the Scene.
     *  \param canvas_id a unique identifier for the Canvas (will be
     *                   automatically populated if not provided).
     *  \param width the width in pixels of the Canvas on the HTML page
     *  \param height the height in pixels of the Canvas on the HTML page
     *  \param html_id optional id of an HTML element to use as this Canvas's
     *                 parent in the HTML DOM (otherwise simply appended to
     *                 document).
     *  \param background_color optional background color for the canvas
     *  \param media_id optional ID of a media file to attach to the canvas.
     *                  This file will be used to drive playback, i.e. frames
     *                  will be displayed in time with the playback of the
     *                  media file.
     *  \return a reference to the new Canvas object
     */
    std::shared_ptr<Canvas2D> create_canvas_2d(
      const std::string& canvas_id = "",
      double width = 400,
      double height = 400,
      const std::string& html_id = "",
      const Color& background_color = Colors::Black,
      const std::string& media_id = "");

    /** Create a new graph canvas and append to the Scene.
     *  \param canvas_id a unique identifier for the Canvas (will be
     *                   automatically populated if not provided).
     *  \param width the width in pixels of the Canvas on the HTML page
     *  \param height the height in pixels of the Canvas on the HTML page
     *  \param name_align How to align the sparkline label (one of 'left',
     *                    'right', 'top', or 'bottom')
     *  \param value_align How to align the sparkline value (one of 'left',
     *                    'right', 'top', or 'bottom')
     *  \param html_id optional id of an HTML element to use as this Canvas's
     *                 parent in the HTML DOM (otherwise simply appended to
     *                 document).
     *  \param background_color optional background color for the canvas
     *  \param margin the outer margin of the graph
     *  \param font_family the font family used for the graph labels
     *  \param name_size the text size in pixels used for the graph labels
     *  \param value_size the text size in pixels used for the graph values
     *  \param media_id optional ID of a media file to attach to the canvas.
     *                  This file will be used to drive playback, i.e. frames
     *                  will be displayed in time with the playback of the
     *                  media file.
     *  \return a reference to the new Graph object
     */
    std::shared_ptr<Graph> create_graph(
      const std::string& canvas_id = "",
      double width = 400,
      double height = 400,
      const std::string& name_align = "left",
      const std::string& value_align = "right",
      const std::string& html_id = "",
      const Color& background_color = Colors::White,
      const Graph::Margin& margin = Graph::Margin(),
      const std::string& font_family = "sans-serif",
      float name_size = 12.0,
      float value_size = 12.0,
      const std::string& media_id = "");

    /** Create a new AudioTrack and add directly to the Scene.
     *  \param audio_id a unique identifier for the AudioTrack (will be
     *                  automatically populated if not provided).
     *  \return a reference to the new AudioTrack object
     */
    std::shared_ptr<AudioTrack> create_audio(const std::string& audio_id = "");

    /** Create a new Video and add directly to the Scene.
     *  \param video_id a unique identifier for the Video (will be
     *                  automatically populated if not provided).
     *  \return a reference to the new Video object
     */
    std::shared_ptr<Video> create_video(const std::string& video_id = "");

    /** Create a new Mesh and append to the Scene.
     *  Meshes are Scene-wide resources that can be reused across multiple
     *  Frames in multiple Canvas3Ds.
     *  \param mesh_id a unique identifier for the Mesh (will be automatically
     *                 populated if not provided).
     *  \param layer_id an identifier to allow several Meshes to be grouped
     *                  together into a "layer".  Layers should be used in
     *                  conjunction with Canvas3D.set_layer_settings().
     *  \param double_sided whether to turn off back face culling and draw the
     *                      Mesh's triangles as double sided.
     *  \param camera_space whether this Mesh is defined in camera space
     *                      (cannot be moved in the ScenePic user interface)
     *                      or world space (standard).
     *  \param shared_color see Mesh constructor.
     *  \param texture_id see Mesh constructor.
     *  \param nn_texture whether the shader should use nearest-neighbor
     *                    texture interpolation.
     *  \param use_texture_alpha whether to use the alpha channel in the
     *                           texture for transparency (only relevant
     *                           for textured Meshes).
     *  \param is_billboard draw this Mesh as a billboard (i.e. always facing
     *                      the user) rather than rotating with the rest of
     *                      the world.
     *  \param is_label this mesh will be treated specially as a label. Not
     *                  for public use.
     *  \return a reference to the new Mesh object
     */
    std::shared_ptr<Mesh> create_mesh(
      const std::string& mesh_id = "",
      const std::string& layer_id = "",
      bool double_sided = false,
      bool camera_space = false,
      const Color& shared_color = Color::None(),
      const std::string& texture_id = "",
      bool nn_texture = true,
      bool use_texture_alpha = false,
      bool is_billboard = false,
      bool is_label = false);

    /** Create a new Mesh by updating a base mesh with new vertex positions.
     *  Meshes are Scene-wide resources that can be reused across multiple
     *  Frames in multiple Canvas3Ds. This method can be used to create a new
     *  mesh which is simply a repositioning of an existing mesh,
     *  thus saving the cost of defining the entire topology every time
     *  the mesh is instantiated. The base mesh is not modified by this
     *  operation.
     *  \param base_mesh_id the id of the base mesh to update
     *  \param positions the new vertex positions
     *  \param mesh_id a unique identifier for the Mesh (will be automatically
     *                 populated if not provided).
     *  \param normals the new vertex normals
     *  \param colors the new vertex colors
     *  \return a references to the new Mesh object
     */
    std::shared_ptr<MeshUpdate> update_mesh(
      const std::string& base_mesh_id,
      const ConstVectorBufferRef& positions,
      const ConstVectorBufferRef& normals,
      const ConstVectorBufferRef& colors,
      const std::string& mesh_id = "");

    /** Create a new Mesh by updating a base mesh with new vertex positions.
     *  Meshes are Scene-wide resources that can be reused across multiple
     *  Frames in multiple Canvas3Ds. This method can be used to create a new
     *  mesh which is simply a repositioning of an existing mesh,
     *  thus saving the cost of defining the entire topology every time
     *  the mesh is instantiated. The base mesh is not modified by this
     *  operation.
     *  \param base_mesh_id the id of the base mesh to update
     *  \param positions the new instance positions
     *  \param rotations the new instance rotations
     *  \param colors the new instance colors
     *  \param mesh_id a unique identifier for the Mesh (will be automatically
     *                 populated if not provided).
     *  \return a references to the new Mesh object
     */
    std::shared_ptr<MeshUpdate> update_instanced_mesh(
      const std::string& base_mesh_id,
      const ConstVectorBufferRef& positions,
      const ConstQuaternionBufferRef& rotations,
      const ConstVectorBufferRef& colors,
      const std::string& mesh_id = "");

    /** \deprecated This method has been deprecated for clarity and naming
     *  purposes.
     *  \sa Scene::update_mesh_positions
     */
    std::shared_ptr<MeshUpdate> update_mesh_without_normals(
      const std::string& base_mesh_id,
      const ConstVectorBufferRef& positions,
      const std::string& mesh_id = "");

    /** Create a new Mesh by updating a base mesh with new vertex positions.
     *  Meshes are Scene-wide resources that can be reused across multiple
     *  Frames in multiple Canvas3Ds. This method can be used to create a new
     *  mesh which is simply a repositioning of an existing mesh, thus saving
     *  the cost of defining the entire topology every time the mesh is
     *  instantiated. The base mesh is not modified by this operation. Normals
     *  will be computed automatically from the provided positions.
     *  \param base_mesh_id the id of the base mesh to update
     *  \param positions the new vertex positions
     *  \param mesh_id a unique identifier for the Mesh (will be automatically
     *                 populated if not provided).
     *  \return a references to the new Mesh object
     */
    std::shared_ptr<MeshUpdate> update_mesh_positions(
      const std::string& base_mesh_id,
      const ConstVectorBufferRef& positions,
      const std::string& mesh_id = "");

    /** Create a new Image and append to the Scene.
     *  Images are Scene-wide resources that can be reused as textures across
     *  multiple Meshes and Frame2Ds.
     *  \param image_id a unique identifier for the Image (will be
     *                  automatically populated if not provided).
     *  \return a reference to the new Image object
     */
    std::shared_ptr<Image> create_image(const std::string& image_id = "");

    /** Create a new Label and append to the Scene.
     *  Labels are Scene-wide resources that can be reused across multiple
     *  Meshes.
     *  \param label_id a unique identifier for the Label (will be
     *                  automatically populated if not provided).
     *  \param text the text to use in the label
     *  \param color the color to use
     *  \param size_in_pixels the size in pixels to display the label at
     *  \param font_family HTML font family name.
     *  \param layer_id an identifier to allow several labels to be grouped
     *                  together into a "layer".  Layers should be used in
     *                  conjunction with set_layer_settings().
     *  \param horizontal_align (one of 'left', 'center', 'right'): how to
     *                          align the label horizontally.
     *  \param vertical_align (one of 'top', 'middle', 'bottom'): how to align
     *                        the label vertically.
     *  \param offset_distance distance in world space to offset the label by
     *                         when using left/right/top/bottom alignment.
     *  \param camera_space whether the label should be created in camera space
     *  \return a reference to the new Label object
     */
    std::shared_ptr<Label> create_label(
      const std::string& label_id = "",
      const std::string& text = "Text",
      const Color& color = Colors::White,
      double size_in_pixels = 12.0,
      const std::string& font_family = "sans-serif",
      const std::string& layer_id = "",
      const std::string& horizontal_align = "left",
      const std::string& vertical_align = "middle",
      double offset_distance = 0.1,
      bool camera_space = false);

    /** Create a new TextPanel and add directly to the HTML page.
     *  \param text_panel_id a unique identifier for the TextPanel (will be
     *                       automatically populated if not provided).
     *  \param html_id optional id of an HTML element to use as this
     *                 TextPanel's parent in the HTML DOM (otherwise will
     *                 simply be appended to document).
     *  \param add_input_box whether to add an input box to the TextPanel
     *  \return a reference to the new TextPanel object
     */
    std::shared_ptr<TextPanel> create_text_panel(
      const std::string& text_panel_id = "",
      const std::string& html_id = "",
      bool add_input_box = false);

    /** Create a new DropDownMenu and add directly to the HTML page.
     *  \param drop_down_menu_id  a unique identifier for the DropDownMenu
     *                            (will be automatically populated if not
     *                            provided).
     *  \param title a title for the drop down box
     *  \param html_id optional id of an HTML element to use as this
     *                 TextPanel's parent in the HTML DOM (otherwise will
     *                 simply be appended to document).
     *  \return an reference to the new DropDownMenu object
     */
    std::shared_ptr<DropDownMenu> create_drop_down_menu(
      const std::string& drop_down_menu_id = "",
      const std::string& title = "",
      const std::string& html_id = "");

    /** Set user interface parameters across all Canvases with given
     *  UIParameters instance.
     *  \param ui_parameters global UI parameters to set
     */
    void configure_user_interface(const UIParameters& ui_parameters);

    /** Specify that all input events should be linked across the provided set
     *  of Canvases.
     *  \param canvas_ids the canvases which should have linked events
     */
    void link_canvas_events(const std::vector<std::string>& canvas_ids);

    /** Simulates one or more keypresses to fire off events in the ScenePic UI
     *  at startup, e.g. video playback.
     *  \param canvas_id ID of the canvas on which to fire off this event.
     *  \param keypresses the key values
     */
    void simulate_key_presses(
      const std::string& canvas_id, const std::vector<std::string>& keypresses);

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** Call this if you are dumping a ScenePic script in multiple parts,
     *  e.g. for client-server interactions, or when chunking a large
     *  visualization into multiple files. Immediately after this, a call
     *  to get_script() will return empty.
     */
    void clear_script();

    /** Returns a JSON-serialized representation of the Scene as a string
     *  If you use clear_script() then this will return updates to the Scene
     *  since the last call to clear_script().
     *  \return a JSON string representing the Scene
     */
    std::string json() const;

    /** Returns a JSONP (https://en.wikipedia.org/wiki/JSONP) script
     *  representing the Scene. If you use clear_script() then this will
     *  return updates to the Scene since the last call to clear_script().
     *  \return a JSONP script representing the Scene
     */
    std::string script() const;

    /**The number of frames per second that will be displayed by this scene. */
    float framerate() const;

    void framerate(float fps);

    /** The CSS visibility of the status bar. */
    const std::string& status_bar_visibility() const;

    void status_bar_visibility(const std::string& visibility);

    /** Save the scene as a JSON file.
     *  To view the JSON, you will need to separately code up the wrapper html
     *  and provide the scenepic.min.js library file. Alternatively, use
     *  save_as_html() to make a self-contained HTML file.
     *  \param path the path to the file on disk
     */
    void save_as_json(const std::string& path) const;

    /** Save the scene as a ScenePic script file (JavaScript JSONP format).
     *  To view the script, you will need to separately code up the wrapper
     *  html. Alternatively, use save_as_html() to make a self-contained
     *  HTML file.
     *  \param path the path to the file on disk
     *  \param standalone whether to include the library in the script
     */
    void save_as_script(const std::string& path, bool standalone = false) const;

    /** Quantize the mesh updates.
     *  Each update will be reduced in size in such a way as to minimize
     *  the expected per-value error from quantization. The number of keyframes
     *  (and thus the level of compression) can be controlled via two
     *  thresholds: the relative error threshold, and the absolute error
     *  threshold. The relative threshold is evaluated as a fraction of
     *  max(positions) - min(positions). The absolute threshold is measured in
     *  raw units.
     *
     *  If the relative error threshold has a positive value but the absolute
     *  error threshold is negative, only the relative threshold will be used.
     *  Conversely, if the relative threshold is negative and the absolute
     *  threshold is positive then only the absolute threshold will be used.
     *  If both are positive, then they will both be applied. If both are
     *  negative, an exception will be raised.
     *
     *  The quantization process will complete in such a way that (quantized -
     *  actual).abs().max() < threshold. In essence, each x, y, and z
     *  coordinate will individually be different by no more than this
     *  threshold after quantization. Thus, a possible result of quantization
     *  will that every frame is a keyframe, i.e. no quantization occurs. More
     *  typically, however, the algorithm finds a few "keyframe" meshes which
     *  minimize the expected error across the remaining (quantized) meshes.
     *  \param relative_error_threshold the maximum expected error as a
     *                                  multiple of the range of values in
     *                                  the base mesh
     *  \param absolute_error_threshold the maximum expected error in absolute
     *                                  units
     *  \param base_mesh_id optional base mesh where quantization is applied.
     *                      Otherwise all mesh updates are quantized.
     *  \param per_frame_range whether to use the most accurate range per
     *                         update, increasing accuracy but reducing
     *                         compression.
     *  \return the per-frame quantization information
     */
    std::map<std::string, QuantizationInfo> quantize_updates(
      float relative_error_threshold = 1e-5,
      float absolute_error_threshold = -1.0,
      const std::string& base_mesh_id = "",
      bool per_frame_range = true);

    /** Returns a breakdown of the number of bytes used by each command type. */
    std::map<std::string, std::size_t> measure_command_size() const;

    /** Save the scene as a html file.
     *  \param path the path to the file on disk
     *  \param title the HTML title
     *  \param head_html the raw HTML to place in the HEAD tag
     *  \param body_html the raw HTML to place in the BODY tag
     *  \param script_path desired relative path for the script. Empty string
     *                     indicates to embed the script in the HTML page.
     *  \param library_path desired relative path for the library. Empty string
     *                      indicates to embed the library in the HTML page.
     */
    void save_as_html(
      const std::string& path,
      const std::string& title = "ScenePic",
      const std::string& head_html = "",
      const std::string& body_html = "",
      const std::string& script_path = "",
      const std::string& library_path = "");

    /** Whether the script has been cleared */
    bool script_cleared() const;

  private:
    template<typename T>
    static void add_commands(
      JsonValue& commands, const std::vector<std::shared_ptr<T>>& entities)
    {
      for (const auto& entity : entities)
      {
        commands.append(entity->to_json());
      }
    }

    float compute_mesh_range(const std::string& mesh_id);

    std::string m_scene_id;
    std::vector<JsonValue> m_display_order;
    std::vector<std::shared_ptr<Canvas3D>> m_canvas3Ds;
    std::vector<std::shared_ptr<Canvas2D>> m_canvas2Ds;
    std::vector<std::shared_ptr<Graph>> m_graphs;
    std::vector<std::shared_ptr<AudioTrack>> m_audios;
    std::vector<std::shared_ptr<Video>> m_videos;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::map<std::string, std::uint32_t> m_update_counts;
    std::vector<std::shared_ptr<MeshUpdate>> m_mesh_updates;
    std::vector<std::shared_ptr<Image>> m_images;
    std::vector<std::shared_ptr<Label>> m_labels;
    std::vector<std::shared_ptr<TextPanel>> m_text_panels;
    std::vector<std::shared_ptr<DropDownMenu>> m_drop_down_menus;
    std::vector<JsonValue> m_misc;
    float m_fps;
    std::string m_status_bar_visibility;

    // These will get out of sync with the above after a call to clear_script()
    // DO NOT REMOVE THESE - they are important
    std::size_t m_num_canvases; // shared UIDs between 2D and 3D
    std::size_t m_num_meshes;
    std::size_t m_num_images;
    std::size_t m_num_audios;
    std::size_t m_num_videos;
    std::size_t m_num_labels;
    std::size_t m_num_text_panels;
    std::size_t m_num_drop_down_menus;
    bool m_script_cleared;
  };
} // namespace scenepic

#endif