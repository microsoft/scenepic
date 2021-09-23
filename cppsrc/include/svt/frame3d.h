// ----------------------------------------------------------------------------
//
// frame3d.h -- Represents a frame of a 3D animation
//
// Copyright (C) 2019 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#ifndef _SCENEPIC_FRAME3D_H_
#define _SCENEPIC_FRAME3D_H_

#include <string>
#include <vector>
#include <algorithm>

#include "camera.h"
#include "json_value.h"
#include "matrix.h"
#include "mesh.h"
#include "mesh_update.h"
#include "label.h"
#include "focus_point.h"

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
    void add_meshes_by_id(const std::vector<std::string> &mesh_ids,
                          const Transform &transform = Transform::Identity());


    /** Add meshes to the frame.
     *  \param meshes the meshes to add
     *  \param transform an optional transform applied to all the meshes
     *  \tparam MeshType a type with a `mesh_id` method.
     */
    template<typename MeshType>
    void add_meshes(const std::vector<std::shared_ptr<MeshType>> &meshes,
                    const Transform &transform = Transform::Identity())
    {
        std::vector<std::string> mesh_ids;
        std::transform(meshes.begin(),
                       meshes.end(),
                       std::back_inserter(mesh_ids),
                       [](const std::shared_ptr<MeshType> &mesh){
                           mesh->mesh_id();
                        });
        this->add_meshes_by_id(mesh_ids, transform);
    }

    /** Add a mesh to the frame.
     *  \param mesh_id the id of the mesh to add
     *  \param transform an optional transform applied to the mesh
     */
    void add_mesh_by_id(const std::string &mesh_id,
                        const Transform &transform = Transform::Identity());


    /** Add a mesh to the frame.
     *  \param mesh the mesh to add
     *  \param transform an optional transform applied to the mesh
     *  \tparam MeshType a type with a `mesh_id` method.
     */
    template<typename MeshType>
    void add_mesh(const std::shared_ptr<MeshType> &mesh,
                  const Transform &transform = Transform::Identity())
    {
        this->add_mesh_by_id(mesh->mesh_id(), transform);
    }


    /** Add a label to the frame.
     *  \param label the label to add
     *  \param position the position at which to put the label
     */
    void add_label(const Label &label, const Vector &position);

    /** The default camera parameters for this frame. */
    const Camera &camera() const;

    /** Set the default camera parameters for this frame.
     *  \param camera the camera parameters
     */
    Frame3D &camera(const Camera &camera);

    /** The focus point of the frame. */
    const FocusPoint &focus_point() const;

    /** Set the focus point of the frame. */
    Frame3D &focus_point(const FocusPoint &focus_point);

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
    Frame3D(const std::string &frame_id,
            const FocusPoint &focus_point = FocusPoint::None(),
            const Camera &camera = Camera::None());

    std::string m_frame_id;
    FocusPoint m_focus_point;
    Camera m_camera;
    std::vector<JsonValue> m_frame_commands;
};
} // namespace scenepic

#endif