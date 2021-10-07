// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_CANVAS3D_H_
#define _SCENEPIC_CANVAS3D_H_

#include "camera.h"
#include "focus_point.h"
#include "frame3d.h"
#include "json_value.h"
#include "layer_settings.h"
#include "mesh.h"
#include "shading.h"
#include "ui_parameters.h"

#include <limits>
#include <map>
#include <memory>
#include <string>

namespace scenepic
{
  /** Represents a ScenePic Canvas3D - a 3D viewport that can contain a number
   * of Frame3Ds. */
  class Canvas3D
  {
  public:
    /** Creates a new Frame3D object and appends to the list of Frame3Ds in the
     *  Canvas3D.
     *  \param frame_id a unique identifier for the Frame3D (will be
     *                  automatically populated if not provided).
     *  \param focus_point optional 3D focus point for this frame (with
     *                     optional 3D rotation for orientation lock), used in
     *                     the ScenePic user interface.
     *  \param mesh_ids optionally pre-populate this Frame3D with a set of
     *                  Meshes, for convenience.
     *  \param camera an optional camera for the frame
     *  \return a new Frame3D object
     */
    std::shared_ptr<Frame3D> create_frame(
      const std::string& frame_id = "",
      const FocusPoint& focus_point = FocusPoint::None(),
      const std::vector<std::string>& mesh_ids = std::vector<std::string>(),
      const Camera& camera = Camera::None());

    /** Creates a new Frame3D object and appends to the list of Frame3Ds in the
     *  Canvas3D.
     *  \param frame_id a unique identifier for the Frame3D (will be
     *                  automatically populated if not provided).
     *  \param meshes optionally pre-populate this Frame3D with a set of Meshes,
     *                for convenience.
     *  \param focus_point optional 3D focus point for this frame (with optional
     *                     3D rotation for orientation lock), used in the
     *                     ScenePic user interface.
     *  \param camera optional camera for the frame
     *  \tparam MeshType a type with a `mesh_id` method.
     *  \return a new Frame3D object
     */
    template<typename MeshType>
    std::shared_ptr<Frame3D> create_frame_with_meshes(
      const std::vector<std::shared_ptr<MeshType>>& meshes,
      const std::string& frame_id = "",
      const FocusPoint& focus_point = FocusPoint::None(),
      const Camera& camera = Camera::None())
    {
      std::vector<std::string> mesh_ids;
      std::transform(
        meshes.begin(),
        meshes.end(),
        std::back_inserter(mesh_ids),
        [](const std::shared_ptr<MeshType>& mesh) { mesh->mesh_id(); });
      return this->create_frame(frame_id, focus_point, mesh_ids, camera);
    }

    /** Specify the visibilities and opacities of certain mesh layers.
     *  Each Mesh object can optionally be part of a user-identified layer
     *  (see Mesh constructor). Calling set_layer_settings will result in an
     *  additional drop-down selector in the ScenePic user interface.
     *  Currently, opacity is only guaranteed to be correct for Meshes
     *  that do not overlap. If you know one mesh should be rendered before
     *  another for opacity purposes, use the renderOrder setting:
     *  low numbers will be drawn before high numbers, and layers with no
     *  renderOrder set will be drawn first. Meshes without layer ids, or with
     *  layer ids that are not specified in this dictionary, will be drawn
     *  filled, without wireframe, at opacity 1.0, and before any layers with
     *  renderOrder set.
     */
    void set_layer_settings(
      const std::map<std::string, LayerSettings> layer_settings);

    /** A unique identifier for the canvas */
    const std::string& canvas_id() const;

    /** Call this if you are dumping a ScenePic script in multiple parts,
     *  e.g. for client-server interactions, or when chunking a large
     *  visualization into multiple files.
     */
    void clear_script();

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

    /** Set of Camera parameters for the Canvas. */
    const Camera& camera() const;

    /** Set of Camera parameters for the Canvas. */
    Canvas3D& camera(const Camera& value);

    /** Set of Shading parameters for the Canvas. */
    const Shading& shading() const;

    /** Set of Shading parameters for the Canvas. */
    Canvas3D& shading(const Shading& value);

    /** The default focus point for the canvas. */
    const FocusPoint& focus_point() const;

    /** The default focus point for the canvas. */
    Canvas3D& focus_point(const FocusPoint& value);

    /** Set of user interface parameters */
    const UIParameters& ui_parameters() const;

    /** Set of user interface parameters */
    Canvas3D& ui_parameters(const UIParameters& value);

    /** The unique ID of the media file associated with this canvas.
     *  This file will be used to drive playback, i.e. frames will be displayed
     *  in time with the playback of the media file.
     */
    const std::string& media_id() const;

    Canvas3D& media_id(const std::string& audio_id);

    /** The width of the canvas */
    double width() const;

    /** The width of the canvas */
    double height() const;

  private:
    friend class Scene;

    /** Constructor.
     *  \param canvas_id a unique identifier for the Canvas
     *  \param width the width of the Canvas
     *  \param height the height of the Canvas
     *  \param camera optional set of Camera parameters for the Canvas.
     *  \param shading optional set of Shading parameters for the Canvas.
     *  \param ui_parameters optional set of user interface parameters
     */
    Canvas3D(
      const std::string& canvas_id,
      double height,
      double width,
      const Camera& camera = Camera(),
      const Shading& shading = Shading(),
      const FocusPoint& focus_point = FocusPoint::None(),
      const UIParameters& ui_parameters = UIParameters());

    std::string m_canvas_id;
    Camera m_camera;
    Shading m_shading;
    FocusPoint m_focus_point;
    UIParameters m_ui_parameters;
    std::map<std::string, LayerSettings> m_layer_settings;
    std::vector<std::shared_ptr<Frame3D>> m_frames;
    std::string m_media_id;
    double m_width;
    double m_height;

    // This will get out of sync with the above after a call to clear_script()
    // DO NOT REMOVE - it is important
    std::size_t m_num_frames;
  };
} // namespace scenepic

#endif