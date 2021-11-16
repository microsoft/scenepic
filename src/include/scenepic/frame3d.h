// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef _SCENEPIC_FRAME3D_H_
#define _SCENEPIC_FRAME3D_H_

#include "camera.h"
#include "focus_point.h"
#include "json_value.h"
#include "label.h"
#include "layer_settings.h"
#include "matrix.h"
#include "mesh.h"
#include "mesh_update.h"

#include <algorithm>
#include <string>
#include <vector>

namespace scenepic
{
  /** Represents a frame of an animation which contains a number of
   *  transformed Mesh objects.
   */
  class Frame3D
  {
  public:
    /** Add meshes to the frame.
     *  \param mesh_ids the ids of the meshes to add
     *  \param transform an optional transform applied to all the meshes
     */
    void add_meshes_by_id(
      const std::vector<std::string>& mesh_ids,
      const Transform& transform = Transform::Identity());

    /** Add meshes to the frame.
     *  \param meshes the meshes to add
     *  \param transform an optional transform applied to all the meshes
     *  \tparam MeshType a type with a `mesh_id` method.
     */
    template<typename MeshType>
    void add_meshes(
      const std::vector<std::shared_ptr<MeshType>>& meshes,
      const Transform& transform = Transform::Identity())
    {
      std::vector<std::string> mesh_ids;
      std::transform(
        meshes.begin(),
        meshes.end(),
        std::back_inserter(mesh_ids),
        [](const std::shared_ptr<MeshType>& mesh) { mesh->mesh_id(); });
      this->add_meshes_by_id(mesh_ids, transform);
    }

    /** Add a mesh to the frame.
     *  \param mesh_id the id of the mesh to add
     *  \param transform an optional transform applied to the mesh
     */
    void add_mesh_by_id(
      const std::string& mesh_id,
      const Transform& transform = Transform::Identity());

    /** Add a mesh to the frame.
     *  \param mesh the mesh to add
     *  \param transform an optional transform applied to the mesh
     *  \tparam MeshType a type with a `mesh_id` method.
     */
    template<typename MeshType>
    void add_mesh(
      const std::shared_ptr<MeshType>& mesh,
      const Transform& transform = Transform::Identity())
    {
      this->add_mesh_by_id(mesh->mesh_id(), transform);
    }

    /** Add a label to the frame.
     *  \param label the label to add
     *  \param position the position at which to put the label
     */
    void add_label(const std::shared_ptr<Label>& label, const Vector& position);

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

    /** The default camera parameters for this frame. */
    const Camera& camera() const;

    /** Set the default camera parameters for this frame.
     *  \param camera the camera parameters
     */
    Frame3D& camera(const Camera& camera);

    /** The focus point of the frame. */
    const FocusPoint& focus_point() const;

    /** Set the focus point of the frame. */
    Frame3D& focus_point(const FocusPoint& focus_point);

    /** Return a JSON string representing the object */
    std::string to_string() const;

    /** Convert this object into ScenePic json.
     *  \return a json value
     */
    JsonValue to_json() const;

  private:
    friend class Canvas3D;

    /** Constructor.
     *  \param frame_id unique identifier for the frame
     *  \param focus_point where the focus point should be located
     *  \param camera the explicit camera parameters for the frame
     */
    Frame3D(
      const std::string& frame_id,
      const FocusPoint& focus_point = FocusPoint::None(),
      const Camera& camera = Camera::None());

    std::string m_frame_id;
    FocusPoint m_focus_point;
    Camera m_camera;
    std::vector<JsonValue> m_frame_commands;
    std::map<std::string, LayerSettings> m_layer_settings;
  };
} // namespace scenepic

#endif