// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "js_lib.h"
#include "scenepic.h"

#include <pybind11/eigen.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <sstream>

using namespace pybind11::literals;
using namespace scenepic;

namespace py = pybind11;

std::string js_lib_src()
{
  std::stringstream stream;
  for (auto& chunk : JS_LIB_SRC)
  {
    stream << chunk;
  }

  return stream.str();
}

PYBIND11_MODULE(_scenepic, m)
{
  py::options options;
  options.disable_function_signatures();

  m.def(
    "js_lib_src",
    &js_lib_src,
    "Returns the Javascript ScenePic library as a string.");

  m.def(
    "ColorFromBytes",
    &Color::from_bytes,
    R"scenepicdoc(
        Construct a color object from byte values.

        Args:
            red: Red channel
            green: Green channel
            blue: Blue channel
        
        Returns:
            np.ndarray: a valid color object
    )scenepicdoc",
    "red"_a,
    "green"_a,
    "blue"_a);

  m.attr("Black") = Colors::Black;
  m.attr("White") = Colors::White;
  m.attr("Red") = Colors::Red;
  m.attr("Maroon") = Colors::Maroon;
  m.attr("Pink") = Colors::Pink;
  m.attr("Brown") = Colors::Brown;
  m.attr("Orange") = Colors::Orange;
  m.attr("Coral") = Colors::Coral;
  m.attr("Olive") = Colors::Olive;
  m.attr("Yellow") = Colors::Yellow;
  m.attr("Beige") = Colors::Beige;
  m.attr("Lime") = Colors::Lime;
  m.attr("Green") = Colors::Green;
  m.attr("Mint") = Colors::Mint;
  m.attr("Teal") = Colors::Teal;
  m.attr("Cyan") = Colors::Cyan;
  m.attr("Navy") = Colors::Navy;
  m.attr("Blue") = Colors::Blue;
  m.attr("Purple") = Colors::Purple;
  m.attr("Lavender") = Colors::Lavender;
  m.attr("Magenta") = Colors::Magenta;
  m.attr("Gray") = Colors::Gray;

  m.def(
    "scale",
    py::overload_cast<float>(&Transforms::scale),
    "scale"_a,
    R"scenepicdoc(
        Creates a 3D homogeneous scale matrix.

        Args:
            scale (float): the scale to use across all axes

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
    )scenepicdoc");

  m.def(
    "scale",
    py::overload_cast<const Vector&>(&Transforms::scale),
    "scale"_a,
    R"scenepicdoc(
        Creates a 3D homogeneous scale matrix.

        Args:
            scale (np.ndarray): the axis-specific scale values
        
        Returns:
            np.ndarray: a 3D homogeneous transform matrix
    )scenepicdoc");

  m.def("translate", &Transforms::translate, "vec"_a, R"scenepicdoc(
        Creates a 3D homogeneous scale matrix.

        Args:
            vec (np.ndarray): the movement vector

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
    )scenepicdoc");

  m.def(
    "rotation_matrix_from_axis_angle",
    &Transforms::rotation_matrix_from_axis_angle,
    "axis"_a,
    "angle"_a,
    R"scenepicdoc(
        Creates a 3D homogeneous scale matrix from axis and angle.

        Args:
            axis (np.ndarray): the axis of rotation
            angle (float): the angle to rotate in radians
    
        Returns:
            np.ndarray: a 3D homogeneous transform matrix
    )scenepicdoc");

  m.def(
    "quaternion_from_axis_angle",
    &Transforms::quaternion_from_axis_angle,
    "axis"_a,
    "angle"_a,
    R"scenepicdoc(
        Convert rotation axis and angle to a quaternion.

        Args:
            axis (np.ndarray): the axis of rotation
            angle (float): the angle to rotate in radians

        Returns:
            np.ndarray: a 3D homogenous transform matrix
    )scenepicdoc");

  m.def(
    "quaternion_multiply",
    &Transforms::quaternion_multiply,
    "a"_a,
    "b"_a,
    R"scenepicdoc(
      Multiply two quaternions together.

      Args:
        a (np.ndarray): the first quaternion
        b (np.ndarray): the second quaternion
      
      Returns:
        np.ndarray: the result of a * b
    )scenepicdoc");

  m.def(
    "axis_angle_to_align_x_to_axis",
    &Transforms::axis_angle_to_align_x_to_axis,
    "axis"_a,
    R"scenepicdoc(
        Find the axis and angle of rotation that will ensure the x-axis aligns with the provided axis.
            
        Note:
            One degree of freedom is underconstrained.

        Args:
            axis (np.ndarray): the axis to which to align the x-axis
        
        Returns:
            np.ndarray: a 3D homogeneous transform matrix
    )scenepicdoc");

  m.def(
    "quaternion_to_align_x_to_axis",
    &Transforms::quaternion_to_align_x_to_axis,
    "axis"_a,
    R"scenepicdoc(
        Creates a quaternion rotation vector that will ensure the x-axis aligns with the provided axis.
        
        Note:
            One degree of freedom is underconstrained.

        Args:
            axis (np.ndarray): the axis to which to align the x-axis
        
        Returns:
            np.ndarray: a quaternion
    )scenepicdoc");

  m.def(
    "rotation_to_align_x_to_axis",
    &Transforms::rotation_to_align_x_to_axis,
    "axis"_a,
    R"scenepicdoc(
        Creates a 3D homogeneous rotation matrix that will ensure the x-axis aligns with the provided axis.
        
        Note:
            One degree of freedom is underconstrained.

        Args:
            axis (np.ndarray): the axis to which to align the x-axis

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
    )scenepicdoc");

  m.def(
    "rotation_about_x", &Transforms::rotation_about_x, "angle"_a, R"scenepicdoc(
        Creates a 3D homogeneous rotation matrix about the x-axis.

        Args:
            angle (float): the angle to rotation in radians
    
        Returns:
            np.ndarray: a 3D homogeous transform matrix
    )scenepicdoc");

  m.def(
    "rotation_about_y", &Transforms::rotation_about_y, "angle"_a, R"scenepicdoc(
        Creates a 3D homogeneous rotation matrix about the y-axis.

        Args:
            angle (float): the angle to rotation in radians
    
        Returns:
            np.ndarray: a 3D homogeous transform matrix
    )scenepicdoc");

  m.def(
    "rotation_about_z", &Transforms::rotation_about_z, "angle"_a, R"scenepicdoc(
        Creates a 3D homogeneous rotation matrix about the z-axis.

        Args:
            angle (float): the angle to rotation in radians
    
        Returns:
            np.ndarray: a 3D homogeous transform matrix
    )scenepicdoc");

  m.def(
    "look_at_rotation",
    &Transforms::look_at_rotation,
    "center"_a,
    "look_at"_a,
    "up_dir"_a,
    R"scenepicdoc(
        Creates a rotation to look at a point in space.

        Description:
            Creates a 3D homogeneous rotation matrix that will cause an object
            with the specified center to look at the specified location, with
            the up direction as given.

        Args:
            center (np.ndarray): the center of the object
            look_at (np.ndarray): the position to look at
            up_dir (np.ndarray): the position that is "up"

        Returns:
            np.ndarray: a 3D homogeneous transform matrix
    )scenepicdoc");

  m.def(
    "euler_angles_to_matrix",
    &Transforms::euler_angles_to_matrix,
    "euler_angles"_a,
    "convention"_a = "XYZ",
    R"scenepicdoc(
        Creates a rotation matrix from three Euler angles.

        Description:
            Creates a 3D homogeneous rotation matrix from three euler angles
            using the specified convention. See https://en.wikipedia.org/wiki/Euler_angles
            for more information on valid conventions. Conventions are specified
            as a three character string indicating the convention order, with
            values taken from [X, Y, Z].

        Args:
            euler_angles (np.ndarray): the Euler angles to use in constructing the matrix
            convention (str, optional): the convention to use in interpreting the angles.
                                        Defaults to "XYZ"
    
        Returns:
            np.ndarray: a 3D homogeneous transform matrix
    )scenepicdoc");

  m.def(
    "quaternion_to_matrix",
    &Transforms::quaternion_to_matrix,
    "quaternion"_a,
    R"scenepicdoc(
        Creates a 3D homogeneous rotation matrix from a quaternion.

        Args:
            quaternion (np.ndarray): the quaternion to use in constructing the matrix
    
        Returns:
            np.ndarray: a 3D homogeneous transform matrix
    )scenepicdoc");

  m.def(
    "gl_projection",
    py::overload_cast<double, double, double, double>(
      &Transforms::gl_projection),
    R"scenepicdoc(
        Creates a WebGL projection transform.

        Description:
            A WebGL projection transform is a matrix that maps from camera view coordinates
            to NDC, or normalized device coordinates. A good description of this can
            be found here: http://ksimek.github.io/2013/06/03/calibrated_cameras_in_opengl/
        
        Args:
            fov_y_degrees (float): the field of view of the camera in degrees
            aspect_ratio (float): the aspect ratio of the camera
            znear (float): the z value of the near clipping plane of the viewing frustum
            zfar (float): the z value of the far clipping plane of the view frustum
    
        Returns:
            np.ndarray: a 3D WebGL projection matrix
    )scenepicdoc",
    "fov_y_degrees"_a,
    "aspect_ratio"_a,
    "znear"_a,
    "zfar"_a);

  m.def(
    "gl_projection",
    py::overload_cast<const Intrinsic&, int, int, double, double>(
      &Transforms::gl_projection),
    R"scenepicdoc(
        Creates a WebGL projection transform.

        Description:
            A WebGL projection transform is a matrix that maps from camera view coordinates
            to NDC, or normalized device coordinates. A good description of this can
            be found here: http://ksimek.github.io/2013/06/03/calibrated_cameras_in_opengl/
        
        Args:
            camera_matrix (np.ndarray): an OpenCV-style camera intrinsic matrix
            width (int): the width of the calibration image
            height (int): the height of the calibration image
            znear (float): the z value of the near clipping plane of the viewing frustum
            zfar (float): the z value of the far clipping plane of the view frustum
    
        Returns:
            np.ndarray: a 3D WebGL projection matrix
    )scenepicdoc",
    "camera_matrix"_a,
    "width"_a,
    "height"_a,
    "znear"_a,
    "zfar"_a);

  m.def(
    "gl_world_to_camera",
    &Transforms::gl_world_to_camera,
    R"scenepicdoc(
        Creates a WebGL view transform.
        
        Description:
            A WebGL view transform goes from world coordinates to camera view coordinates.
            This method will create one from an OpenCV-style camera extrinsic matrix.

        Args:
            extrinsic (np.ndarray): the camera-to-world matrix, which places the camera
                                    in the scene

        Returns:
            np.ndarray: a WebGL view matrix
    )scenepicdoc",
    "extrinsic"_a);

  py::class_<Camera>(m, "Camera", R"scenepicdoc(
        A camera type
        
        Note:
            The camera can be created either with the look at paradigm (center,
            look_at, up_dir), with center and rotation, or with an explicit
            world-to-camera matrix.

        Args:
            center (np.ndarray, optional): the camera center. Defaults to [0, 0, 4].
            look_at (np.ndarray, optional): where the camera is looking. Defaults to [0, 0, 0].
            up_dir (np.ndarray, optional): the direction that is "up" for the camera. Defaults to [0, 1, 0].
            fov_y_degrees (float, optional): vertical field of view in degrees. Defaults to 45.0.
            near_crop_distance (float, optional): the near clipping plane of the view frustum. Defaults to 0.01.
            far_crop_distance (float, optional): the far clipping plane of the view frustum. Defaults to 20.0.
            rotation (np.ndarray, optional): a homogeneous rotation transform.
            world_to_camera (np.ndarray, optional): a homogeneous 3D transform mapping world coordinates to camera space.
            projection (np.ndarray, optional): the WebGL projection matrix from camera view coordinates to NDC.
            aspect_ratio (float, optional): The aspect ratio of the camera. Defaults to 1.0.
    )scenepicdoc")
    .def(
      py::init<const Transform&, double, double, double, double>(),
      "world_to_camera"_a,
      "fov_y_degrees"_a = 45.0,
      "near_crop_distance"_a = 0.01,
      "far_crop_distance"_a = 20.0,
      "aspect_ratio"_a = 1.0)
    .def(
      py::
        init<const Vector&, const Transform&, double, double, double, double>(),
      "center"_a,
      "rotation"_a,
      "fov_y_degrees"_a = 45.0,
      "near_crop_distance"_a = 0.01,
      "far_crop_distance"_a = 20.0,
      "aspect_ratio"_a = 1.0)
    .def(
      py::init<
        const Vector&,
        const Vector&,
        const Vector&,
        double,
        double,
        double,
        double>(),
      "center"_a = Vector(0, 0, 4),
      "look_at"_a = Vector(0, 0, 0),
      "up_dir"_a = Vector(0, 1, 0),
      "fov_y_degrees"_a = 45.0,
      "near_crop_distance"_a = 0.01,
      "far_crop_distance"_a = 20.0,
      "aspect_ratio"_a = 1.0)
    .def(
      py::init<const Transform&, const Transform&>(),
      "world_to_camera"_a,
      "projection"_a)
    .def_property(
      "aspect_ratio",
      py::overload_cast<>(&Camera::aspect_ratio, py::const_),
      py::overload_cast<float>(&Camera::aspect_ratio),
      "float: Update the aspect ratio of the camera")
    .def_property_readonly(
      "center", &Camera::center, "np.ndarray: The camera center")
    .def_property_readonly(
      "look_at", &Camera::look_at, "np.ndarray: Where the camera is looking")
    .def_property_readonly(
      "up_dir",
      &Camera::up_dir,
      "np.ndarray: The direction that is \"up\" for the camera")
    .def_property_readonly(
      "rotation", &Camera::rotation, "np.ndarray: The rotation matrix")
    .def_property_readonly(
      "world_to_camera",
      &Camera::world_to_camera,
      "np.ndarray: Homogeneous 3D transform mapping world coordinates to "
      "camera space")
    .def_property_readonly(
      "camera_to_world",
      &Camera::camera_to_world,
      "np.ndarray: Homogeneous 3D transform that places the camera in the "
      "scene")
    .def_property_readonly(
      "projection",
      &Camera::projection,
      "np.ndarray: WebGL projection matrix from camera view coordinates to NDC")
    .def("__repr__", &Camera::to_string);

  m.def(
    "camera_orbit",
    &Camera::orbit,
    R"scenepicdoc(
        Creates a sequence of cameras which move in an orbit around the origin.
       
        Args:
            num_frames (int): the number of frames in the sequence
            distance (float): the distance of the camera from the origin
            num_times (int): the number of times to orbit
            min_altitude (float): the minimum altitude of the orbit
            max_altitude (float): the maximum altitude of the orbit
            up_dir (np.ndarray): the direct that is up (as a normalized vector)
            forward_dir (np.ndarray): the direction that is forward (as a normalized vector)
            fov_y_degrees (float): the field of the view of the camera in degrees
            aspect_ratio (float): the aspect ratio of the camera
            near_crop_distance (float): the near clipping plane of the view frustum
            far_crop_distance (float): the far clipping plane of the view frustum
    
        Returns:
            List[Camera]: a sequence of cameras moving in an orbit
    )scenepicdoc",
    "num_frames"_a,
    "distance"_a,
    "num_times"_a,
    "min_altitude"_a = 0.25,
    "max_altitude"_a = 0.8,
    "up_dir"_a = Vector(0, 1, 0),
    "forward_dir"_a = Vector(0, 0, 1),
    "fov_y_degrees"_a = 45.0,
    "aspect_ratio"_a = 1.0,
    "near_crop_distance"_a = 0.01,
    "far_crop_distance"_a = 20.0);

  py::class_<LoopSubdivisionStencil, std::shared_ptr<LoopSubdivisionStencil>>(
    m, "LoopSubdivisionStencil", R"scenepicdoc(
        This class enables efficient Loop subdivision of triangle meshes.

        The subdivision stencil specifies what linear combinations of existing vertices are needed, and
        stencils can be reused across meshes with different vertex positions but identical triangle topologies.
        Thus, the constructor only takes the triangles, and caches the stencil.
        The stencil can be applied to a set of vertices using the apply() method.
    )scenepicdoc")
    .def(
      "apply",
      &LoopSubdivisionStencil::apply,
      R"scenepicdoc(
            Applies the stencil to a vertex buffer.
            
            Description:
                This method takes a set of vertices and subdivides them (using the stencil) to
                create the vertices of a more detailed mesh.
            
            Args:
                vertices (np.ndarray): a (N, 3) buffer of vertices
            
            Returns:
                np.ndarray: a (S, 3) buffer of subdivided vertices
        )scenepicdoc",
      "vertices"_a)
    .def_property_readonly(
      "triangles",
      &LoopSubdivisionStencil::triangles,
      "np.ndarray: The triangles of the subdivided mesh")
    .def_property_readonly(
      "vertex_count",
      &LoopSubdivisionStencil::vertex_count,
      "int: The number of vertices in the subdivided mesh")
    .def_property_readonly(
      "triangle_count",
      &LoopSubdivisionStencil::triangle_count,
      "int: The number of triangles in the subdivided mesh")
    .def_static(
      "create",
      &LoopSubdivisionStencil::create,
      R"scenepicdoc(
            Creates a new stencil by subdividing the provided triangles.

            Args:
                triangles (np.ndarray): the initial triangle indices
                steps (int, optional): specifies how many steps of subdivision to apply. Defaults to 1.
                project_to_limit (bool, optional): specifies whether the vertices should be projected onto the
                                                   limit surface in the final step of subdivision. Defaults to false.
            
            Return:
                LoopSubdivisionStencil
            )scenepicdoc",
      "triangles"_a,
      "steps"_a = 1,
      "project_to_limit"_a = false);

  py::class_<MeshInfo, std::shared_ptr<MeshInfo>>(m, "MeshInfo", R"scenepicdoc(
        Class which holds information needed to construct a mesh object, like
        the vertex positions, the triangulation, and either per-vertex UV coordinates
        or Color (if present).
    )scenepicdoc")
    .def_property_readonly(
      "positions",
      py::overload_cast<>(&MeshInfo::position_buffer),
      "np.ndarray: A reference to the position buffer.")
    .def_property_readonly(
      "normals",
      py::overload_cast<>(&MeshInfo::normal_buffer),
      "np.ndarray: A reference to the normal buffer")
    .def_property_readonly(
      "triangles",
      py::overload_cast<>(&MeshInfo::triangle_buffer),
      "np.ndarray: A reference to the triangle buffer.")
    .def_property_readonly(
      "uvs",
      py::overload_cast<>(&MeshInfo::uv_buffer),
      "np.ndarray: A reference to the uv buffer.")
    .def_property_readonly(
      "colors",
      py::overload_cast<>(&MeshInfo::color_buffer),
      "np.ndarray: A reference to the color buffer.")
    .def_property_readonly(
      "has_normals",
      &MeshInfo::has_normals,
      "Whether the mesh info contains vertex normals.")
    .def(
      "subdivide",
      &MeshInfo::subdivide,
      R"scenepicdoc(
            Subdivide this mesh using loop subdivision.

            Args:
                steps (int, optional): specifies how many steps of subdivision to apply. Defaults to 1.
                project_to_limit (bool, optional): specifies whether the vertices should be projected onto the
                                                   limit surface in the final step of subdivision. Defaults to false.
        
            Returns:
                MeshInfo: a subdivided version of this mesh
        )scenepicdoc",
      "steps"_a = 1,
      "project_to_limit"_a = false);

  m.def(
    "load_obj",
    py::overload_cast<const std::string&>(load_obj),
    "path"_a,
    R"scenepicdoc(
        Loads a WaveFront OBJ file from disk as a MeshInfo object.

        Args:
            path (str): the path to the OBJ file on disk

        Returns:
            MeshInfo: contains the positions, a triangulation, and UVs (if present)
    )scenepicdoc");

  py::class_<Mesh, std::shared_ptr<Mesh>>(m, "Mesh", R"scenepicdoc(
        The basic ScenePic mesh class, containing vertex, triangle, and line buffers.
        To allow for compatibility with Numpy, we use row major order, so each
        data point is stored as a row for efficiency. Vertex buffer contains
        float32 elements with structure: x,y,z, and the normal buffer
        contains elements: nx,ny,nz.
 
        If shared_color is set then per-vertex color [cr,cg,cb,ca] in the
        color buffer is omitted to save memory.
 
        If texture_id is None then per-vertex texture coordinates [tu, tv] in
        the uv buffer are omitted to save memory.
 
        Right handed coordinate system used throughout.
 
        Triangle winding order is counter clockwise.

        Args:
            shared_color (Color, optional): shared color for all objects in the mesh. Defaults to None.
            texture_id (str, optional): The ID of the Image to use as a texture. Defaults to None.
    )scenepicdoc")
    .def(
      py::init<const Color&, const std::string&>(),
      "A Mesh created in this way will not be added to the scene.",
      "shared_color"_a = Color::None(),
      "texture_id"_a = "")
    .def("__repr__", &Mesh::to_string)
    .def(
      "count_vertices",
      &Mesh::count_vertices,
      "The number of vertices in the mesh.")
    .def_property_readonly(
      "center_of_mass",
      &Mesh::center_of_mass,
      "np.ndarray: The mean of mesh vertex positions")
    .def("reverse_triangle_order", &Mesh::reverse_triangle_order, R"scenepicdoc(
            Reverses the winding order of all triangles in this mesh.
            Useful when interoping with existing codebases that use opposite convention
        )scenepicdoc")
    .def(
      "apply_transform",
      &Mesh::apply_transform,
      R"scenepicdoc(
            Apply a 3D homogeneous matrix transform (i.e. 4x4 matrix) to all vertices
            (and appropriately to the normals) in the Mesh.

            Args:
                transform (np.ndarray): a 3D homogeneous transform
        )scenepicdoc",
      "transform"_a)
    .def(
      "apply_rotation",
      &Mesh::apply_rotation,
      R"scenepicdoc(
            Apply a 3D homogeneous matrix rotation (i.e. 4x4 matrix) to all vertices
            (and appropriately to the normals) in the Mesh.
            Separated out from apply_transform() so that a matrix inverse is not required, for speed.

            Args:
                transform (np.ndarray): a 3D homogeneous rotation matrix
        )scenepicdoc",
      "transform"_a)
    .def(
      "get_transformed",
      &Mesh::get_transformed,
      R"scenepicdoc(
            Get a transformed copy of this Mesh.

            Args:
                transform (np.ndarray): a 3D homogeneous transform

            Returns:
                Mesh: a transformed copy of this mesh
        )scenepicdoc",
      "transform"_a)
    .def(
      "get_rotated",
      &Mesh::get_rotated,
      R"scenepicdoc(
            Get a rotated copy of this Mesh.

            Args:
                transform (np.ndarray): a 3D homogeneous transform
            
            Returns:
                Mesh: a transformed copy of this mesh
        )scenepicdoc",
      "transform"_a)
    .def(
      "append_mesh",
      &Mesh::append_mesh,
      R"scenepicdoc(
            Append another mesh object to this mesh

            Args:
                mesh (Mesh): the mesh to append
        )scenepicdoc",
      "mesh"_a)
    .def(
      "add_triangle",
      &Mesh::add_triangle,
      R"scenepicdoc(
            Adds a triangle to the mesh.

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                p0 (np.ndarray, optional): the first vertex. Defaults to [0, 0, 0].
                p1 (np.ndarray, optional): the second vertex. Defaults to [1, 0, 0].
                p2 (np.ndarray, optional): the third vertex. Defaults to [0, 1, 0].
                normal (np.ndarray, optional): will be computed automatically if None. Defaults to None.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.             
                uv_0 (np.ndarray, optional): first UV coordinate (for UV meshes). Defaults to (0, 0).
                uv_1 (np.ndarray, optional): second UV coordinate (for UV meshes). Defaults to (1, 0).
                uv_2 (np.ndarray, optional): third UV coordinate (for UV meshes). Defaults to (0, 1).
        )scenepicdoc",
      "color"_a = Color::None(),
      "p0"_a = Vector(0, 0, 0),
      "p1"_a = Vector(1, 0, 0),
      "p2"_a = Vector(0, 1, 0),
      "normal"_a = VectorNone(),
      "fill_triangles"_a = true,
      "add_wireframe"_a = false,
      "uv_0"_a = UV(0, 0),
      "uv_1"_a = UV(1, 0),
      "uv_2"_a = UV(0, 1))
    .def(
      "add_quad",
      &Mesh::add_quad,
      R"scenepicdoc(
            Adds a quad to the mesh.
            All points are assumed to lie on the same plane.

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                p0 (np.ndarray, optional): the first vertex. Defaults to [0, -0.5, -0.5].
                p1 (np.ndarray, optional): the second vertex. Defaults to [0, 0.5, -0.5].
                p2 (np.ndarray, optional): the third vertex. Defaults to [0, 0.5, 0.5].
                p3 (np.ndarray, optional): the fourth vertex. Defaults to [0, -0.5, 0.5].
                normal (np.ndarray, optional): will be computed automatically if None. Defaults to None.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
                transform (np.ndarray, optional): 3D homogeneous transform matrix. Defaults to None.
                uv_0 (np.ndarray, optional): first UV coordinate (for UV meshes). Defaults to (0, 0).
                uv_1 (np.ndarray, optional): second UV coordinate (for UV meshes). Defaults to (1, 0).
                uv_2 (np.ndarray, optional): third UV coordinate (for UV meshes). Defaults to (1, 1).
                uv_3 (np.ndarray, optional): fourth UV coordinate (for UV meshes). Defaults to (0, 1).
        )scenepicdoc",
      "color"_a = Color::None(),
      "p0"_a = Vector(0, -0.5f, -0.5f),
      "p1"_a = Vector(0, 0.5f, -0.5f),
      "p2"_a = Vector(0, 0.5f, 0.5f),
      "p3"_a = Vector(0, -0.5f, 0.5f),
      "normal"_a = VectorNone(),
      "fill_triangles"_a = true,
      "add_wireframe"_a = false,
      "transform"_a = Transform::Identity(),
      "uv_0"_a = UV(0, 0),
      "uv_1"_a = UV(1, 0),
      "uv_2"_a = UV(1, 1),
      "uv_3"_a = UV(0, 1))
    .def(
      "add_image",
      &Mesh::add_image,
      R"scenepicdoc(
            Add an image as a textured quad to this Mesh.
            The Mesh must have a texture_id.

            Args:
                origin (np.ndarray, optional): bottom left corner of image. Defaults to (-0.5, -0.5, 0).
                x_axis (np.ndarray, optional): direction and length of the x-axis of the image in world space.
                                               Defaults to (1, 0, 0).
                y_axis (np.ndarray, optional): direction and length of the y-axis of the image in world space.
                                               Defaults to (0, 1, 0).
                normal (np.ndarray, optional): will be computed automatically if None. Defaults to None.
                uv_0 (np.ndarray, optional): the bottom left uv coordinate. Defaults to (0, 0).
                uv_1 (np.ndarray, optional): the top left uv coordinate. Defaults to (1, 0).
                uv_2 (np.ndarray, optional): the tp right uv coordinate. Defaults to (1, 1).
                uv_3 (np.ndarray, optional): the bottom right uv coordinate. Defaults to (0, 1).
                double_sided (bool, optional): whether the image should be shown on both sides. Defaults to True.
                transform (np.ndarray, optional): a 3D homogeneous transform matrix. Defaults to None.
        )scenepicdoc",
      "origin"_a = Vector(-0.5f, -0.5f, 0),
      "x_axis"_a = Vector(1, 0, 0),
      "y_axis"_a = Vector(0, 1, 0),
      "normal"_a = VectorNone(),
      "uv_0"_a = UV(0, 0),
      "uv_1"_a = UV(1, 0),
      "uv_2"_a = UV(1, 1),
      "uv_3"_a = UV(0, 1),
      "double_sided"_a = true,
      "transform"_a = Transform::Identity())
    .def(
      "add_cube",
      &Mesh::add_cube,
      R"scenepicdoc(
            Add a unit diameter cube to this mesh.

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                transform (np.ndarray, optional): homogeneous 3D transform matrix. Defaults to None.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "color"_a = Color::None(),
      "transform"_a = Transform::Identity(),
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_thickline",
      &Mesh::add_thickline,
      R"scenepicdoc(
            Add a thick line to this Mesh.

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                start_point (np.ndarray, optional): the start point of the line. Defaults to [-0.5, 0, 0].
                end_point (np.ndarray, optional): the end point of the line. Defaults to [0.5, 0, 0].
                start_thickness (float, optional): the start thickness of the line. Defaults to 0.1.
                end_thickness (float, optional): the end thickness of the line. Defaults to 0.1.
                transform (np.ndarray, optional): homogeneous 3D transform matrix. Defaults to None.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "color"_a = Color::None(),
      "start_point"_a = Vector(-0.5, 0, 0),
      "end_point"_a = Vector(0.5, 0, 0),
      "start_thickness"_a = 0.1f,
      "end_thickness"_a = 0.1f,
      "transform"_a = Transform::Identity(),
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_cone",
      &Mesh::add_cone,
      R"scenepicdoc(
            Add a cone to this Mesh. The cone has base center at [0.5, 0., 0.]
            and apex at [-0.5, 0., 0.].

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
                truncation_height (float, optional): draws a truncated cone of this height. Defaults to 1.
                lat_count (int, optional): number of discrete samples in latitude. Defaults to 10.
                long_count (int, optional): number of discrete samples in longitude. Defaults to 10.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "color"_a = Color::None(),
      "transform"_a = Transform::Identity(),
      "truncation_height"_a = 1,
      "lat_count"_a = 10,
      "long_count"_a = 10,
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_coordinate_axes",
      &Mesh::add_coordinate_axes,
      R"scenepicdoc(
            Add a set of coordinate axes to this Mesh. xyz axes map to RGB colors.

            Args:
                length (float, optional): length of axes. Defaults to 1.0.
                thickness (float, optional): thickness of axes. Defaults to 0.1.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
        )scenepicdoc",
      "length"_a = 1.0f,
      "thickness"_a = 0.1f,
      "transform"_a = Transform::Identity())
    .def(
      "add_camera_frustum",
      py::overload_cast<
        const Color&,
        float,
        float,
        float,
        float,
        const Transform&>(&Mesh::add_camera_frustum),
      R"scenepicdoc(
            Add a camera frustum visualization to this Mesh.

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                fov_y_degrees (float, optional): vertical field of view in degrees. Defaults to 45.0.
                aspect_ratio (float, optional): aspect ratio of field of view (width/height). Defaults to 16/9.
                depth (float, optional): depth of the frustum. Defaults to 1.0.
                thickness (float, optional): thickness of the edges. Defaults to 0.02.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
        )scenepicdoc",
      "color"_a = Color::None(),
      "fov_y_degrees"_a = 45.0f,
      "aspect_ratio"_a = 16.0f / 9.0f,
      "depth"_a = 1.0f,
      "thickness"_a = 0.02f,
      "transform"_a = Transform::Identity())
    .def(
      "add_camera_frustum",
      py::overload_cast<const Camera&, const Color&, float, float>(
        &Mesh::add_camera_frustum),
      R"scenepicdoc(
            Add a camera frustum visualization to this Mesh.

            Args:
                camera (Camera, optional): the camera to use for the frustum
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                thickness (float, optional): thickness of the edges. Defaults to 0.02.
                depth (float, optional): depth of the frustum. Defaults to 1.0.
        )scenepicdoc",
      "camera"_a,
      "color"_a = Color::None(),
      "thickness"_a = 0.02f,
      "depth"_a = 1.0f)
    .def(
      "add_camera_image",
      &Mesh::add_camera_image,
      R"scenepicdoc(
             Add the image plane for a camera into the scene.

             Args:
                camera (Camera, optional): the ScenePic Camera corresponding to the image. 
                depth (float, optional): depth of the frustum. Defaults to 1.0.
        )scenepicdoc",
      "camera"_a,
      "depth"_a = 1.0f)
    .def(
      "add_disc",
      &Mesh::add_disc,
      R"scenepicdoc(
            Add a x-axis aligned unit diameter disc to this Mesh.

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
                segment_count (int, optional): number of segments in the disc. Defaults to 10.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "color"_a = Color::None(),
      "transform"_a = Transform::Identity(),
      "segment_count"_a = 10,
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_cylinder",
      &Mesh::add_cylinder,
      R"scenepicdoc(
            Add a x-axis aligned unit diameter cylinder to this Mesh.

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
                segment_count (int, optional): number of segments in the cylinder. Defaults to 10.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "color"_a = Color::None(),
      "transform"_a = Transform::Identity(),
      "segment_count"_a = 10,
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_sphere",
      &Mesh::add_sphere,
      R"scenepicdoc(
            Add a default (ico) unit diameter sphere primitive to the mesh.

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "color"_a = Color::None(),
      "transform"_a = Transform::Identity(),
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_icosphere",
      &Mesh::add_icosphere,
      R"scenepicdoc(
            Add a unit diameter ico-sphere to this Mesh

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
                steps (int, optional): number of subdivision steps to apply to the icosahedron base primitive. Defaults to 0.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "color"_a = Color::None(),
      "transform"_a = Transform::Identity(),
      "steps"_a = 0,
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_uv_sphere",
      &Mesh::add_uv_sphere,
      R"scenepicdoc(
            Add a unit diameter UV sphere to this Mesh.

            Description:
                If the mesh has a texture attached, this method will automatically create
                a set of vertex UVs which unwraps the sphere to map to the full UV space.

            Args:
                color (np.ndarray, optional): required unless Mesh was constructed with shared_color argument. Defaults to None.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
                lat_count (int, optional): number of discrete samples in latitude. Defaults to 10.
                long_count (int, optional): number of discrete samples in longitude. Defaults to 10.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "color"_a = Color::None(),
      "transform"_a = Transform::Identity(),
      "lat_count"_a = 10,
      "long_count"_a = 10,
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_mesh",
      &Mesh::add_mesh,
      R"scenepicdoc(
            Add a triangle mesh to this ScenePic Mesh, with normals computed automatically.

            Args:
                mesh_info (MeshInfo): object containing the vertices, triangles, and uvs/colors.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
                reverse_triangle_order (bool, optional): whether to reverse the triangle winding order. Defaults to False.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "mesh_info"_a,
      "transform"_a = Transform::Identity(),
      "reverse_triangle_order"_a = false,
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_mesh_without_normals",
      &Mesh::add_mesh_without_normals,
      R"scenepicdoc(
            Add a triangle mesh to this ScenePic Mesh, with normals computed automatically.

            Args:
                vertices (np.ndarray): float32 matrix of [N, 3] vertex positions
                triangles (np.ndarray): uint32 matrix of [N, 3] triangle vertex indices
                colors (np.ndarray, optional): float32 matrix of [N, 3] per-vertex RGB color vectors. Defaults to None.
                uvs (np.ndarray, optional): float32 matrix of [N, 2] per-vertex UV coordinates. Defaults to None.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
                reverse_triangle_order (bool, optional): whether to reverse the triangle winding order. Defaults to False.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "vertices"_a,
      "triangles"_a,
      "colors"_a = ColorBufferNone(),
      "uvs"_a = UVBufferNone(),
      "transform"_a = Transform::Identity(),
      "reverse_triangle_order"_a = false,
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_mesh_with_normals",
      &Mesh::add_mesh_with_normals,
      R"scenepicdoc(
            Add a triangle mesh to this ScenePic Mesh.

            Args:
                vertices (np.ndarray): float32 matrix of [N, 3] vertex positions
                normals (np.ndarray): float32 matrix of [N, 3] vertex normals
                triangles (np.ndarray): uint32 matrix of [N, 3] triangle vertex indices
                colors (np.ndarray, optional): float32 matrix of [N, 3] per-vertex RGB color vectors. Defaults to None.
                uvs (np.ndarray, optional): float32 matrix of [N, 2] per-vertex UV coordinates. Defaults to None.
                transform (np.ndarray, optional): homogeneous 3D transform. Defaults to None.
                reverse_triangle_order (bool, optional): whether to reverse the triangle winding order. Defaults to False.
                fill_triangles (bool, optional): whether to fill the primitive. Defaults to True.
                add_wireframe (bool, optional): whether to add a wireframe outline. Defaults to False.
        )scenepicdoc",
      "vertices"_a,
      "normals"_a,
      "triangles"_a,
      "colors"_a = ColorBufferNone(),
      "uvs"_a = UVBufferNone(),
      "transform"_a = Transform::Identity(),
      "reverse_triangle_order"_a = false,
      "fill_triangles"_a = true,
      "add_wireframe"_a = false)
    .def(
      "add_lines",
      &Mesh::add_lines,
      R"scenepicdoc(
            Add a line cloud to this Mesh.

            Args:
                start_points (np.ndarray): float32 matrix of [N, 3] line start positions (or [N, 6] positions with colors)
                end_points (np.ndarray): float32 matrix of [N, 3] line end positions (or [N, 6] positions with colors)
                color (np.ndarray, optional): if provided, will override any per-point colors stored in start_colors and end_colors.
                                              Defaults to None.
                transform (np.ndarray): homogeneous 3D transform. Defaults to None.
        )scenepicdoc",
      "start_points"_a,
      "end_points"_a,
      "color"_a = Color::None(),
      "transform"_a = Transform::Identity())
    .def(
      "enable_instancing",
      &Mesh::enable_instancing,
      R"scenepicdoc(
            Makes ScenePic render this Mesh with multiple instances, e.g. for point-cloud visualizations.
            Can be used to make point clouds, for example.
            The whole contents of the Mesh will be rendered multiple times.

            Args:
                positions (np.ndarray): float32 matrix of [N, 3] instance positions
                rotations (np.ndarray, optional): float32 matrix of [N, 4] per-point quaternion rotations. Defaults to None.
                colors (np.ndarray, optional): float32 matrix of [N, 3] colors. If provided, overrides a Mesh's shared_color
                                               or per-vertex color. Defaults to None.
        )scenepicdoc",
      "positions"_a,
      "rotations"_a = QuaternionBufferNone(),
      "colors"_a = ColorBufferNone())
    .def_property(
      "shared_color",
      py::overload_cast<>(&Mesh::shared_color, py::const_),
      py::overload_cast<const Color&>(&Mesh::shared_color),
      "np.ndarray: Whole-mesh color (reduces memory requirements but makes "
      "Mesh monochrome).")
    .def_property(
      "texture_id",
      py::overload_cast<>(&Mesh::texture_id, py::const_),
      py::overload_cast<const std::string&>(&Mesh::texture_id),
      "str: Specifies the id of a ScenePic Image to be used as a texture in "
      "this Mesh.")
    .def_property_readonly(
      "mesh_id", &Mesh::mesh_id, "str: A unique identifier for the Mesh")
    .def_property(
      "layer_id",
      py::overload_cast<>(&Mesh::layer_id, py::const_),
      py::overload_cast<const std::string&>(&Mesh::layer_id),
      R"scenepicdoc(
                          str: An identifier to allow several Meshes to be grouped
                          together into a "layer".  Layers should be used in
                          conjunction with Canvas3D.set_layer_settings().
                      )scenepicdoc")
    .def_property(
      "double_sided",
      py::overload_cast<>(&Mesh::double_sided, py::const_),
      py::overload_cast<bool>(&Mesh::double_sided),
      "bool: Whether to turn off back face culling and draw the Mesh's "
      "triangles as double sided.")
    .def_property(
      "camera_space",
      py::overload_cast<>(&Mesh::camera_space, py::const_),
      py::overload_cast<bool>(&Mesh::camera_space),
      R"scenepicdoc(
                          bool: Whether this Mesh is defined in camera space (cannot be moved in the ScenePic user
                          interface) or world space (standard).
                      )scenepicdoc")
    .def_property(
      "nn_texture",
      py::overload_cast<>(&Mesh::nn_texture, py::const_),
      py::overload_cast<bool>(&Mesh::nn_texture),
      "bool: Whether the shader should use nearest-neighbor texture "
      "interpolation.")
    .def_property(
      "use_texture_alpha",
      py::overload_cast<>(&Mesh::use_texture_alpha, py::const_),
      py::overload_cast<bool>(&Mesh::use_texture_alpha),
      R"scenepicdoc(
                          bool: Whether to use the alpha channel in the texture for transparency
                          (only relevant for textured Meshes).
                      )scenepicdoc")
    .def_property(
      "is_billboard",
      py::overload_cast<>(&Mesh::is_billboard, py::const_),
      py::overload_cast<bool>(&Mesh::is_billboard),
      R"scenepicdoc(
                          bool: Draw this Mesh as a billboard (i.e. always facing the user) rather than
                          rotating with the rest of the world.
                      )scenepicdoc")
    .def("get_vertex_buffer", &Mesh::vertex_buffer, R"scenepicdoc(
                          Returns a reference to the contents vertex buffer. 
                          Vertices are stored in a NxD matrix in the following way per row:
                          [x, y, z, nx, ny, nz] (no color or uvs)
                          [x, y, z, nx, ny, nz, r, g, b] (+ color)
                          [x, y, z, nx, ny, nz, u, v] (+ uv)
                      )scenepicdoc");

  py::class_<MeshUpdate, std::shared_ptr<MeshUpdate>>(
    m, "MeshUpdate", "A new mesh formed by updated an existing mesh.")
    .def("__repr__", &MeshUpdate::to_string)
    .def_property_readonly(
      "base_mesh_id",
      &MeshUpdate::base_mesh_id,
      "str: The unique identifier for the base mesh")
    .def_property_readonly(
      "mesh_id",
      &MeshUpdate::mesh_id,
      "str: The unique identifier of the newly created mesh")
    .def_property_readonly(
      "frame_index",
      &MeshUpdate::frame_index,
      "int: The position in the creation order for this update")
    .def(
      "quantize_",
      &MeshUpdate::quantize,
      "keyframe_index"_a,
      "range"_a,
      "keyframe_vertex_buffer"_a)
    .def("difference_range_", &MeshUpdate::difference_range, "vertex_buffer"_a)
    .def("get_vertex_buffer", &MeshUpdate::vertex_buffer, R"scenepicdoc(
                          Returns a reference to the contents vertex buffer. 
                          Vertices are stored in a NxD matrix in the following way per row:
                          [x, y, z, nx, ny, nz] (no color or uvs)
                          [x, y, z, nx, ny, nz, r, g, b] (+ color)
                          [x, y, z, nx, ny, nz, u, v] (+ uv)
                      )scenepicdoc");

  py::class_<AudioTrack, std::shared_ptr<AudioTrack>>(
    m, "AudioTrack", "A ScenePic AudioTrack type")
    .def("__repr__", &AudioTrack::to_string)
    .def(
      "load",
      &AudioTrack::load,
      R"scenepicdoc(
            Load an audio file from the disk

            Args:
                path (str): the path to the audio file
        )scenepicdoc",
      "path"_a)
    .def_property_readonly(
      "audio_id",
      &AudioTrack::audio_id,
      "str: A unique identifier for the audio track")
    .def(
      "data_get_",
      py::overload_cast<>(&AudioTrack::data),
      "Gets the encoded binary audio data")
    .def(
      "data_set_",
      py::overload_cast<const std::vector<std::uint8_t>&>(&AudioTrack::data),
      "Sets the encoded binary audio data");

  py::class_<Video, std::shared_ptr<Video>>(m, "Video", "A ScenePic Video type")
    .def("__repr__", &Video::to_string)
    .def(
      "load",
      &Video::load,
      R"scenepicdoc(
            Load a video file from the disk

            Args:
                path (str): the path to the video file
        )scenepicdoc",
      "path"_a)
    .def_property_readonly(
      "video_id", &Video::video_id, "str: A unique identifier for the video")
    .def(
      "data_get_",
      py::overload_cast<>(&Video::data),
      "Gets the encoded binary video data")
    .def(
      "data_set_",
      py::overload_cast<const std::vector<std::uint8_t>&>(&Video::data),
      "Sets the encoded binary video data");

  py::class_<Image, std::shared_ptr<Image>>(m, "Image", "A ScenePic Image type")
    .def("__repr__", &Image::to_string)
    .def(
      "load",
      &Image::load,
      R"scenepicdoc(
            Load an image file from the disk

            Args:
                path (str): the path to the image file
        )scenepicdoc",
      "path"_a)
    .def_property_readonly(
      "image_id", &Image::image_id, "str: A unique identifier for the image")
    .def(
      "data_get_",
      py::overload_cast<>(&Image::data),
      "Gets the encoded binary image data")
    .def(
      "data_set_",
      py::overload_cast<const std::vector<std::uint8_t>&>(&Image::data),
      "Sets the encoded binary image data")
    .def_property(
      "ext",
      py::overload_cast<>(&Image::ext, py::const_),
      py::overload_cast<const std::string&>(&Image::ext),
      "str: The extension of the image (e.g. JPG, PNG)");

  py::class_<Label, std::shared_ptr<Label>>(m, "Label", "A ScenePic label type")
    .def("__repr__", &Label::to_string)
    .def_property_readonly(
      "label_id", &Label::label_id, "str: A unique identifier for the Label")
    .def_property_readonly(
      "mesh_id", &Label::mesh_id, "str: The mesh associated with this label")
    .def_property(
      "text",
      py::overload_cast<>(&Label::text, py::const_),
      py::overload_cast<const std::string&>(&Label::text),
      "str: The text to use in the label.")
    .def_property(
      "fill_color",
      py::overload_cast<>(&Label::fill_color, py::const_),
      py::overload_cast<const Color&>(&Label::fill_color),
      "np.ndarray: The color to use.")
    .def_property(
      "size_in_pixels",
      py::overload_cast<>(&Label::size_in_pixels, py::const_),
      py::overload_cast<double>(&Label::size_in_pixels),
      "float: The size in pixels to display the label at.")
    .def_property(
      "font_family",
      py::overload_cast<>(&Label::font_family, py::const_),
      py::overload_cast<const std::string&>(&Label::font_family),
      "str: HTML font family name.")
    .def_property(
      "horizontal_align",
      py::overload_cast<>(&Label::horizontal_align, py::const_),
      py::overload_cast<const std::string&>(&Label::horizontal_align),
      "str: How to align the label horizontally (one of 'left', 'center', "
      "'right')")
    .def_property(
      "vertical_align",
      py::overload_cast<>(&Label::vertical_align, py::const_),
      py::overload_cast<const std::string&>(&Label::vertical_align),
      "str: How to align the label vertically (one of 'top', 'middle', "
      "'bottom')")
    .def_property(
      "offset_distance",
      py::overload_cast<>(&Label::offset_distance, py::const_),
      py::overload_cast<double>(&Label::offset_distance),
      "float: Distance in world space to offset the label by when using "
      "left/right/top/bottom alignment.");

  py::class_<FocusPoint>(m, "FocusPoint", R"scenepicdoc(
        3D focus point for this frame (with optional 3D
        rotation for orientation lock), used in the ScenePic user interface.
    )scenepicdoc")
    .def("__repr__", &FocusPoint::to_string)
    .def(
      py::init<const Vector&, const Vector&>(),
      "position"_a = VectorNone(),
      "orientation_axis_angle"_a = VectorNone())
    .def_property_readonly(
      "position",
      &FocusPoint::position,
      "np.ndarray: The position of the focus point")
    .def_property_readonly(
      "orientation_axis_angle",
      &FocusPoint::orientation_axis_angle,
      "np.ndarray: The orientation axis angle");

  py::class_<Frame2D, std::shared_ptr<Frame2D>>(
    m, "Frame2D", "Represents a frame of a 2D animation")
    .def("__repr__", &Frame2D::to_string)
    .def(
      "add_line",
      &Frame2D::add_line,
      R"scenepicdoc(
            Add a line to the frame.
     
            Args:
                coordinates (np.ndarray): float32 matrix of [N, 2] coordinates of the line
                line_color (np.ndarray, optional): the color of the line. Defaults to Black.
                line_width (float, optional): the width of the line. Defaults to 1.0.
                fill_color (np.ndarray, optional): the fill color to use. Defaults to None.
                close_path (bool, optional): whether to close the path. Defaults to False.
                layer_id (str, optional): Unique ID of the layer associated with this primitive. Defaults to "".
        )scenepicdoc",
      "coordinates"_a,
      "line_color"_a = Colors::Black,
      "line_width"_a = 1.0f,
      "fill_color"_a = Color::None(),
      "close_path"_a = false,
      "layer_id"_a = "")
    .def(
      "add_rectangle",
      &Frame2D::add_rectangle,
      R"scenepicdoc(
            Add a rectangle to the frame.

            Args:
                x (float): the upper left corner x coordinate
                y (float): the upper left corner y coordinate
                w (float): the width of the rectangle
                h (float): the height of the rectangle
                line_color (np.ndarray, optional): the color of the line. Defaults to Black.
                line_width (float, optional): the width of the line. Defaults to 1.0.
                fill_color (np.ndarray, optional): the fill color to use. Defaults to None.
                layer_id (str, optional): Unique ID of the layer associated with this primitive. Defaults to "".
        )scenepicdoc",
      "x"_a,
      "y"_a,
      "w"_a,
      "h"_a,
      "line_color"_a = Colors::Black,
      "line_width"_a = 1.0f,
      "fill_color"_a = Color::None(),
      "layer_id"_a = "")
    .def(
      "add_circle_",
      &Frame2D::add_circle,
      R"scenepicdoc(
        )scenepicdoc",
      "x"_a,
      "y"_a,
      "radius"_a,
      "line_color"_a = Colors::Black,
      "line_width"_a = 1.0f,
      "fill_color"_a = Color::None(),
      "layer_id"_a = "")
    .def(
      "add_image_",
      &Frame2D::add_image,
      "image_id"_a,
      "position_type"_a = "fit",
      "x"_a = 0.0f,
      "y"_a = 0.0f,
      "scale"_a = 1.0f,
      "smoothed"_a = false,
      "layer_id"_a = "")
    .def(
      "add_video",
      &Frame2D::add_video,
      R"scenepicdoc(
            Add an frame from a video to the frame.

            Args:
                position_type (str, optional): one of "fit", "fill", "stretch", or "manual". Defaults to "fit".
                x (float, optional): x coordinate to draw video at if position_type == "manual". Defaults to 0.0.
                y (float, optional): y coordinate to draw video at if position_type == "manual". Defaults to 0.0.
                scale (float, optional): scale factor to draw video by if position_type == "manual". Defaults to 1.0.
                smoothed (bool, optional): whether to draw an video with smoothing or using nearest neighbor. Defaults to False.
                layer_id (str, optional): Unique ID of the layer associated with this primitive. Defaults to "".
        )scenepicdoc",
      "position_type"_a = "fit",
      "x"_a = 0.0f,
      "y"_a = 0.0f,
      "scale"_a = 1.0f,
      "smoothed"_a = false,
      "layer_id"_a = "")
    .def(
      "add_text_",
      &Frame2D::add_text,
      "text"_a,
      "left"_a,
      "bottom"_a,
      "color"_a = Colors::White,
      "size_in_pixels"_a = 12.0f,
      "font_family"_a = "sans-serif",
      "layer_id"_a = "");

  py::class_<Frame3D, std::shared_ptr<Frame3D>>(m, "Frame3D", R"scenepicdoc(
        Represents a frame of an animation which contains a number of
        transformed Mesh objects.
    )scenepicdoc")
    .def("__repr__", &Frame3D::to_string)
    .def(
      "add_meshes_",
      &Frame3D::add_meshes_by_id,
      R"scenepicdoc(
                Add meshes to the frame.

                Args:
                    mesh_ids (List[str]): the ids of the meshes to add
                    transform (np.ndarray, optional): homogeneous 3D transform applied to all the meshes. Defaults to None.
             )scenepicdoc",
      "mesh_ids"_a,
      "transform"_a = Transform::Identity())
    .def(
      "add_mesh_",
      &Frame3D::add_mesh_by_id,
      R"scenepicdoc(
                Add a mesh to the frame.

                Args:
                    mesh_id (str): the id of the mesh to add
                    transform (np.ndarray, optional): homogeneous 3D transform applied to the mesh. Defaulst to None.
             )scenepicdoc",
      "mesh_id"_a,
      "transform"_a = Transform::Identity())
    .def(
      "add_label",
      &Frame3D::add_label,
      R"scenepicdoc(
            Add a label to the frame.

            Args:
                label (Label): the label to add
                position (np.ndarray): the float32 [x, y, z] position at which to put the label
        )scenepicdoc",
      "label"_a,
      "position"_a)
    .def(
      "set_layer_settings_",
      &Frame3D::set_layer_settings,
      R"scenepicdoc(
            Specify the visibilities and opacities of certain mesh layers.
            Each Mesh object can optionally be part of a user-identified layer
            (see Mesh constructor). Calling set_layer_settings will result in an
            additional drop-down selector in the ScenePic user interface.
        )scenepicdoc",
      "layer_settings"_a)
    .def_property(
      "focus_point",
      py::overload_cast<>(&Frame3D::focus_point, py::const_),
      py::overload_cast<const FocusPoint&>(&Frame3D::focus_point),
      "FocusPoint: The focus point of the frame")
    .def_property(
      "camera",
      py::overload_cast<>(&Frame3D::camera, py::const_),
      py::overload_cast<const Camera&>(&Frame3D::camera),
      R"scenepicdoc(
            Camera: The camera for the frame. This property can be used to create cinematic
            camera movement within a ScenePic, but the user can choose to override the camera
            track at any time. If they reset to the original camera, however, it will make
            subsequent frames use the specified camera parameters.)scenepicdoc");

  py::class_<Shading>(m, "Shading", R"scenepicdoc(
        Parameters of the shaders

        Args:
            bg_color (np.ndarray, optional): background color. Defaults to Black.
            ambient_light_color (np.ndarray, optional): color of ambient light. Defaults to [0.7, 0.7, 0.7].
            directional_light_color (np.ndarray, optional): color of directional light. Defaults to [0.3, 0.3, 0.3].
            directional_light_dir (np.ndarray, optional): direction of directional light. Defaults to [2, 1, 2].
    )scenepicdoc")
    .def("__repr__", &Shading::to_string)
    .def(
      py::init<const Color&, const Color&, const Color&, const Vector&>(),
      "bg_color"_a = Color(0, 0, 0),
      "ambient_light_color"_a = Color(0.7f, 0.7f, 0.7f),
      "directional_light_color"_a = Color(0.3f, 0.3f, 0.3f),
      "directional_light_dir"_a = Vector(2, 1, 2))
    .def(
      py::init<const Color4&, const Color&, const Color&, const Vector&>(),
      "bg_color"_a = Color4(0, 0, 0, 1.0),
      "ambient_light_color"_a = Color(0.7f, 0.7f, 0.7f),
      "directional_light_color"_a = Color(0.3f, 0.3f, 0.3f),
      "directional_light_dir"_a = Vector(2, 1, 2))
    .def_property_readonly(
      "bg_color", &Shading::bg_color, "np.ndarray: the background color")
    .def_property_readonly(
      "ambient_light_color",
      &Shading::ambient_light_color,
      "np.ndarray: the color of ambient light")
    .def_property_readonly(
      "directional_light_color",
      &Shading::directional_light_color,
      "np.ndarray: the directional light color")
    .def_property_readonly(
      "directional_light_dir",
      &Shading::directional_light_dir,
      "np.ndarray: the direction of the directional light");

  py::class_<UIParameters>(m, "UIParameters", R"scenepicdoc(
        Per-Canvas3D parameters of the ScenePic user interface.

        Args:
            pointer_alt_key_multiplier (float, optional): when the Alt key is pressed, mouse movement
                                                          is scaled by this factor. Defaults to 0.2.
            pointer_rotation_speed (float, optional): linear scale factor of pointer translation
                                                      into camera rotation. Defaults to 0.01.
            mouse_wheel_translation_speed (float, optional): linear scale factor of mouse wheel input
                                                             into camera translation. Defaults to 0.005.
            layer_dropdown_visibility (str, optional): CSS visibility setting for the layer dropdown menu.
                                                       Defaults to "visible".
    )scenepicdoc")
    .def("__repr__", &UIParameters::to_string)
    .def(
      py::init<double, double, double, double, const std::string&>(),
      "pointer_alt_key_multiplier"_a = 0.2,
      "pointer_rotation_speed"_a = 0.01,
      "mouse_wheel_translation_speed"_a = 0.005,
      "key_press_speed"_a = 0.1,
      "layer_dropdown_visibility"_a = "visible")
    .def_property_readonly(
      "pointer_alt_key_multiplier",
      &UIParameters::pointer_alt_key_multiplier,
      "float: When the Alt key is pressed, mouse movement is scaled by this "
      "factor.")
    .def_property_readonly(
      "pointer_rotation_speed",
      &UIParameters::pointer_rotation_speed,
      "float: linear scale factor of pointer translation into camera rotation.")
    .def_property_readonly(
      "mouse_wheel_translation_speed",
      &UIParameters::mouse_wheel_translation_speed,
      "float: linear scale factor of mouse wheel input into camera "
      "translation.")
    .def_property_readonly(
      "key_press_speed",
      &UIParameters::key_down_speed,
      "float: movement speed from key presses")
    .def_property_readonly(
      "layer_dropdown_visibility",
      &UIParameters::layer_dropdown_visibility,
      "str: the CSS visibility setting for the layer dropdown menu");

  py::class_<Canvas2D, std::shared_ptr<Canvas2D>>(m, "Canvas2D", R"scenepicdoc(
        A 2D canvas that can contain a number of Frame2Ds.
    )scenepicdoc")
    .def("__repr__", &Canvas2D::to_string)
    .def_property_readonly(
      "canvas_id",
      &Canvas2D::canvas_id,
      "str: A unique identifier for the canvas")
    .def(
      "create_frame",
      &Canvas2D::create_frame,
      R"scenepicdoc(
            Creates a new Frame2D object and appends to the list of Frame2Ds in
            the Canvas2D.
            
            Args:
                frame_id (str): a unique identifier for the Frame2D (will be
                                automatically populated if not provided).
            
            Returns:
                Frame2D: a new Frame2D object
        )scenepicdoc",
      "frame_id"_a = "")
    .def(
      "set_layer_settings_",
      &Canvas2D::set_layer_settings,
      R"scenepicdoc(
            Specify the visibilities and opacities of certain primitives.
            Each Primitive object can optionally be part of a user-identified layer.
            Calling set_layer_settings will result in an
            additional drop-down selector in the ScenePic user interface.
        )scenepicdoc",
      "layer_settings"_a)
    .def("clear_script", &Canvas2D::clear_script, R"scenepicdoc(
            Call this if you are dumping a ScenePic script in multiple parts,
            e.g. for client-server interactions, or when chunking a large
            visualization into multiple files.
        )scenepicdoc")
    .def_property_readonly(
      "width", &Canvas2D::width, "float: The width of the Canvas")
    .def_property_readonly(
      "height", &Canvas2D::height, "float: The height of the Canvas")
    .def_property(
      "background_color",
      py::overload_cast<>(&Canvas2D::background_color, py::const_),
      py::overload_cast<const Color&>(&Canvas2D::background_color),
      "np.ndarray: The background color of the frame")
    .def_property(
      "media_id",
      py::overload_cast<>(&Canvas2D::media_id, py::const_),
      py::overload_cast<const std::string&>(&Canvas2D::media_id),
      R"scenepicdoc(
                          str: The ID of the attached media file.
                          
                          This file will be used to drive playback, i.e. frames will be
                          displayed in time with the playback of the media file.
                      )scenepicdoc");

  py::class_<LayerSettings>(m, "LayerSettings", R"scenepicdoc(
        Settings used for customizing canvas drawing by layer.
        Currently, opacity is only guaranteed to be correct for Meshes
        that do not overlap.
        If you know one mesh should be rendered before another for opacity purposes,
        use the renderOrder setting: low numbers will be drawn before high numbers,
        and layers with no renderOrder set will be drawn last. Meshes without layer ids,
        or with layer ids that are not specified in this dictionary, will be drawn filled,
        without wireframe, at opacity 1.0, and after any layers with renderOrder set.

        Args:
            filled (bool, optional): whether to draw the layer with filled polygons. Defaults to True.
            wireframe (bool, optional): whether to draw the layer with a frame outline. Defaults to False.
            opacity (float, optional): the opacity with which to draw the layer. Defaults to 1.0.
            render_order (int, optional): Where in the transparency order to draw this layer. Defaults to None.
    )scenepicdoc")
    .def("__repr__", &LayerSettings::to_string)
    .def(
      py::init<bool, bool, double, int>(),
      "filled"_a = true,
      "wireframe"_a = false,
      "opacity"_a = 1.0,
      "render_order"_a = LayerSettings::NO_RENDER_ORDER)
    .def_property(
      "filled",
      py::overload_cast<>(&LayerSettings::filled, py::const_),
      py::overload_cast<bool>(&LayerSettings::filled),
      "bool: Specifies whether to draw the layer with filled triangles")
    .def_property(
      "wireframe",
      py::overload_cast<>(&LayerSettings::wireframe, py::const_),
      py::overload_cast<bool>(&LayerSettings::wireframe),
      "bool: Specifies whether to draw the layer with a wireframe outline")
    .def_property(
      "opacity",
      py::overload_cast<>(&LayerSettings::opacity, py::const_),
      py::overload_cast<double>(&LayerSettings::opacity),
      "float: In [0,1] specifies the opacity with which to draw the layer")
    .def_property(
      "render_order",
      py::overload_cast<>(&LayerSettings::render_order, py::const_),
      py::overload_cast<int>(&LayerSettings::render_order),
      "int: Specifies a fixed order to draw transparency.");

  py::class_<Canvas3D, std::shared_ptr<Canvas3D>>(m, "Canvas3D", R"scenepicdoc(
        Represents a ScenePic Canvas3D - a 3D viewport that can contain a number of Frame3Ds.
    )scenepicdoc")
    .def(
      "create_frame_",
      &Canvas3D::create_frame,
      R"scenepicdoc(
            Creates a new Frame3D object and appends to the list of Frame3Ds in the Canvas3D.

            Args:
                frame_id (str, optional): a unique identifier for the Frame3D (will be
                                          automatically populated if not provided). Defaults to None.
                focus_point (FocusPoint, optional): optional 3D focus point for this frame (with optional 3D
                                                    rotation for orientation lock), used in the ScenePic user interface.
                                                    Defaults to None.
                mesh_ids (List[str], optional): optionally pre-populate this Frame3D with a set of Meshes,
                                                for convenience. Defaults to None.
                camera (Camera, optional): optional default camera for the frame. Defaults to None.
            
            Returns:
                Frame3D: a new Frame3D object
        )scenepicdoc",
      "frame_id"_a = "",
      "focus_point"_a = FocusPoint::None(),
      "mesh_ids"_a = std::vector<std::string>(),
      "camera"_a = Camera::None())
    .def("__repr__", &Canvas3D::to_string)
    .def_property_readonly(
      "canvas_id",
      &Canvas3D::canvas_id,
      "str: A unique identifier for the canvas")
    .def(
      "set_layer_settings_",
      &Canvas3D::set_layer_settings,
      R"scenepicdoc(
            Specify the visibilities and opacities of certain mesh layers.
            Each Mesh object can optionally be part of a user-identified layer
            (see Mesh constructor). Calling set_layer_settings will result in an
            additional drop-down selector in the ScenePic user interface.
        )scenepicdoc",
      "layer_settings"_a)
    .def("clear_script", &Canvas3D::clear_script, R"scenepicdoc(
            Call this if you are dumping a ScenePic script in multiple parts,
            e.g. for client-server interactions, or when chunking a large
            visualization into multiple files.
        )scenepicdoc")
    .def_property_readonly(
      "width", &Canvas3D::width, "float: The width of the Canvas")
    .def_property_readonly(
      "height", &Canvas3D::height, "float: The height of the Canvas")
    .def_property(
      "camera",
      py::overload_cast<>(&Canvas3D::camera, py::const_),
      py::overload_cast<const Camera&>(&Canvas3D::camera),
      "Camera: Set of Camera parameters for the Canvas.")
    .def_property(
      "shading",
      py::overload_cast<>(&Canvas3D::shading, py::const_),
      py::overload_cast<const Shading&>(&Canvas3D::shading),
      "Shading: Set of Shading parameters for the Canvas.")
    .def_property(
      "focus_point",
      py::overload_cast<>(&Canvas3D::focus_point, py::const_),
      py::overload_cast<const FocusPoint&>(&Canvas3D::focus_point),
      "FocusPoint: Default focus point for the canvas.")
    .def_property(
      "ui_parameters",
      py::overload_cast<>(&Canvas3D::ui_parameters, py::const_),
      py::overload_cast<const UIParameters&>(&Canvas3D::ui_parameters),
      "UIParameters: Set of user interface parameters")
    .def_property(
      "media_id",
      py::overload_cast<>(&Canvas3D::media_id, py::const_),
      py::overload_cast<const std::string&>(&Canvas3D::media_id),
      R"scenepicdoc(
                          str: The ID of the attached media file.
                          
                          This file will be used to drive playback, i.e. frames will be
                          displayed in time with the playback of the media file.
                      )scenepicdoc");

  py::class_<Graph::Margin>(m, "Margin", R"scenepicdoc(
        Represents the margin along the edges of a graph.

        Args:
            size (float, optional): space in pixels to use for all margins. Default 10.
            top (float, optional): the top margin value in pixels
            right (float, optional): the right margin value in pixels
            bottom (float, optional): the bottom margin value in pixels
            left (float, optional): the left margin value in pixels
    )scenepicdoc")
    .def(py::init<>())
    .def(py::init<double>(), "size"_a)
    .def(
      py::init<double, double, double, double>(),
      "top"_a,
      "right"_a,
      "bottom"_a,
      "left"_a)
    .def("__repr__", &Graph::Margin::to_string)
    .def_readwrite(
      "top", &Graph::Margin::top, "float: The top margin in pixels")
    .def_readwrite(
      "right", &Graph::Margin::right, "float: The right margin in pixels")
    .def_readwrite(
      "bottom", &Graph::Margin::bottom, "float: The bottom margin in pixels")
    .def_readwrite(
      "left", &Graph::Margin::left, "float: The left margin in pixels");

  py::class_<Graph::VerticalRule>(m, "VerticalRule", R"scenepicdoc(
        Represents a vertical rule drawn on a sparkline graph.

        Args:
          frame (int): Frame to add the vertical rule
          color: (Color, optional): the line color. Defauts to black.
          line_width: (float, optional): The line width in pixels. Defaults to 1.0f.
  )scenepicdoc")
    .def(
      py::init<std::int64_t, const Color&, float>(),
      "frame"_a,
      "color"_a = scenepic::Colors::Black,
      "line_width"_a = 1.0f)
    .def_readwrite(
      "frame",
      &Graph::VerticalRule::frame,
      "int: the frame at which to add the line")
    .def_readwrite(
      "color", &Graph::VerticalRule::color, "Color: the color of the line")
    .def_readwrite(
      "line_width",
      &Graph::VerticalRule::line_width,
      "float: the width of the line in pixels");

  py::class_<Graph, std::shared_ptr<Graph>>(
    m, "Graph", "A 2D viewport that animates one or more sparklines.")
    .def("__repr__", &Graph::to_string)
    .def_property_readonly(
      "canvas_id", &Graph::canvas_id, "str: A unique identifier for the canvas")
    .def_property(
      "background_color",
      py::overload_cast<>(&Graph::background_color, py::const_),
      py::overload_cast<const Color&>(&Graph::background_color),
      "np.ndarray: The background color of the frame")
    .def_property(
      "margin",
      py::overload_cast<>(&Graph::margin, py::const_),
      py::overload_cast<const Graph::Margin&>(&Graph::margin),
      "Margin: The outside margin of the graph")
    .def_property(
      "font_family",
      py::overload_cast<>(&Graph::font_family, py::const_),
      py::overload_cast<const std::string&>(&Graph::font_family),
      "str: The font family used for the graph labels")
    .def_property(
      "name_size",
      py::overload_cast<>(&Graph::name_size, py::const_),
      py::overload_cast<float>(&Graph::name_size),
      "float: The size of the sparkline labels in pixels")
    .def_property(
      "value_size",
      py::overload_cast<>(&Graph::value_size, py::const_),
      py::overload_cast<float>(&Graph::value_size),
      "float: The size of the sparkline labels in pixels")
    .def_property(
      "name_align",
      py::overload_cast<>(&Graph::name_align, py::const_),
      py::overload_cast<const std::string&>(&Graph::name_align),
      R"scenepicdoc(
        The alignment of the sparkline labels.
        One of ('left', 'right', 'top', 'bottom', or 'none')
      )scenepicdoc")
    .def_property(
      "value_align",
      py::overload_cast<>(&Graph::value_align, py::const_),
      py::overload_cast<const std::string&>(&Graph::value_align),
      R"scenepicdoc(
        The alignment of the sparkline values.
        One of ('left', 'right', 'top', 'bottom', or 'none')
      )scenepicdoc")
    .def_property(
      "media_id",
      py::overload_cast<>(&Graph::media_id, py::const_),
      py::overload_cast<const std::string&>(&Graph::media_id),
      R"scenepicdoc(
                          str: The ID of the attached media file.
                          
                          This file will be used to drive playback, i.e. frames will be
                          displayed in time with the playback of the media file.
                      )scenepicdoc")
    .def(
      "add_sparkline",
      &Graph::add_sparkline,
      R"scenepicdoc(
            Adds a sparkline to the graph.

            Args:
                name (str): the name of the measured quantity
                values (np.ndarray): the measured values
                line_color (Color, optional): the color of the line (and its labels). Defaults to Black.
                line_width (float, optional): The width of the line. Defaults to 1.0.    
                vertical_rules (List[VerticalRule], optional): the vertical rules to add. Defaults to None.
        )scenepicdoc",
      "name"_a,
      "values"_a,
      "line_color"_a = Colors::Black,
      "line_width"_a = 1.0f,
      "vertical_rules"_a = std::vector<Graph::VerticalRule>{});

  py::class_<DropDownMenu, std::shared_ptr<DropDownMenu>>(
    m, "DropDownMenu", "Represents a ScenePic DropDownMenu UI component.")
    .def("__repr__", &DropDownMenu::to_string)
    .def_property(
      "title",
      py::overload_cast<>(&DropDownMenu::title, py::const_),
      py::overload_cast<const std::string&>(&DropDownMenu::title),
      "str: The title of the dropdown menu.")
    .def_property(
      "items",
      py::overload_cast<>(&DropDownMenu::items, py::const_),
      py::overload_cast<const std::vector<std::string>&>(&DropDownMenu::items),
      "List[str]: The items in the dropdown menu.")
    .def_property(
      "selection",
      py::overload_cast<>(&DropDownMenu::selection, py::const_),
      py::overload_cast<int>(&DropDownMenu::selection),
      "The currently selected item.")
    .def(
      "set_disabled",
      &DropDownMenu::set_disabled,
      "index"_a,
      "disabled"_a,
      "Sets the disabled flag on an option.")
    .def("clear_script", &DropDownMenu::clear_script, R"scenepicdoc(
            Call this if you are dumping a ScenePic script in multiple parts,
            e.g. for client-server interactions, or when chunking a large
            visualization into multiple files.
        )scenepicdoc");

  py::class_<QuantizationInfo>(
    m, "QuantizationInfo", "Information about the results of quantization")
    .def("__repr__", &QuantizationInfo::to_string)
    .def_readonly(
      "keyframe_count",
      &QuantizationInfo::keyframe_count,
      "int: The number of keyframes used.")
    .def_readonly(
      "estimated_size_ratio",
      &QuantizationInfo::estimated_size_ratio,
      "float: The estimated size ratio after compression.")
    .def_readonly(
      "mean_error",
      &QuantizationInfo::mean_error,
      "float: The mean per-frame error.")
    .def_readonly(
      "max_error",
      &QuantizationInfo::max_error,
      "float: The maximum per-frame error.");

  py::class_<TextPanel, std::shared_ptr<TextPanel>>(
    m, "TextPanel", "Represents a ScenePic TextPanel UI component.")
    .def("__repr__", &TextPanel::to_string)
    .def_property(
      "title",
      py::overload_cast<>(&TextPanel::title, py::const_),
      py::overload_cast<const std::string&>(&TextPanel::title),
      "str: The title of the text panel.")
    .def_property(
      "text",
      py::overload_cast<>(&TextPanel::text, py::const_),
      py::overload_cast<const std::string&>(&TextPanel::text),
      "str: The text within the text panel.")
    .def_property(
      "input_text",
      py::overload_cast<>(&TextPanel::input_text, py::const_),
      py::overload_cast<const std::string&>(&TextPanel::input_text),
      "str: Text for the optional input textbox.")
    .def("clear_script", &TextPanel::clear_script, R"scenepicdoc(
            Call this if you are dumping a ScenePic script in multiple parts,
            e.g. for client-server interactions, or when chunking a large
            visualization into multiple files.
        )scenepicdoc");

  py::class_<Scene>(m, "Scene", R"scenepicdoc(
        Top level container representing an entire ScenePic Scene.

        Args:
            scene_id (str, optional): a unique identifier for the Scene (will be
                                      automatically populated if not provided). Defaults to None.
    )scenepicdoc")
    .def(py::init<const std::string&>(), "scene_id"_a = "")
    .def(
      "grid",
      &Scene::grid,
      R"scenepicdoc(
            Defines a CSS layout grid for the canvases of the ScenePic. 
            
            Description:
                Should not be used in conjunction with `html_id` for individual canvases. See 
                https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Grid_Layout for more information.
            
            Args:
                width (str): CSS declaration for the width of the ScenePic.
                grid_template_rows (str): CSS declaration defining the rows of the ScenePic
                grid_template_columns (str): CSS declaration defining the columns of the ScenePic
        )scenepicdoc",
      "width"_a,
      "grid_template_rows"_a,
      "grid_template_columns"_a)
    .def(
      "place",
      &Scene::place,
      R"scenepicdoc(
            Places a canvas at a specific place in the ScenePic grid.
            
            Note:
                For use with Scene.grid
            
            Args:
                canvas_id (str): The unique ID of the canvas to place
                grid_row (str): the CSS row specification for the Canvas
                grid_column (str): the CSS column specification for the Canvas
        )scenepicdoc",
      "canvas_id"_a,
      "grid_row"_a,
      "grid_column"_a)
    .def(
      "create_canvas_3d",
      &Scene::create_canvas_3d,
      R"scenepicdoc(
            Create a new 3D Canvas and append to the Scene.
            
            Args:
                canvas_id (str, optional): a unique identifier for the Canvas (will be
                                           automatically populated if not provided). Defaults to None.
                width (int, optional): the width in pixels of the Canvas on the HTML page. Defaults to 400.
                height (int, optional): the height in pixels of the Canvas on the HTML page. Defaults to 400.
                html_id (str, optional): id of an HTML element to use as this
                                         Canvas's parent in the HTML DOM (otherwise simply
                                         appended to document). Defaults to None.
                camera (Camera, optional): Camera parameters for the Canvas. See `Camera` for defaults.
                shading (Shading, optional): Set of Shading parameters for the Canvas. See `Shading` for defaults.
                ui_parameters (UIParameters, optional): Set of user interface parameters. See `UIParameters` for
                                                        defaults.
                media_id (str, optional): optional ID of a media file to attach to the canvas. This file will be
                                          used to drive playback, i.e. frames will be displayed in time with
                                          the playback of the media file.

            Returns:
                Canvas3D: a reference to the new Canvas object
        )scenepicdoc",
      "canvas_id"_a = "",
      "width"_a = 400,
      "height"_a = 400,
      "html_id"_a = "",
      "camera"_a = Camera::None(),
      "shading"_a = Shading(),
      "ui_parameters"_a = UIParameters(),
      "media_id"_a = "")
    .def(
      "create_canvas_2d",
      &Scene::create_canvas_2d,
      R"scenepicdoc(
            Create a new 2D Canvas and append to the Scene.

            Args:
                canvas_id (str, optional): a unique identifier for the Canvas (will be
                                           automatically populated if not provided). Defaults to None.
                width (int, optional): the width in pixels of the Canvas on the HTML page. Defaults to 400.
                height (int, optional): the height in pixels of the Canvas on the HTML page. Defaults to 400.
                html_id (str, optional): id of an HTML element to use as this
                                         Canvas's parent in the HTML DOM (otherwise simply
                                         appended to document). Defaults to None.
                background_color (np.ndarray, optional): the background color of the canvas. Defaults to Black.
                media_id (str, optional): optional ID of a media file to attach to the canvas. This file will be
                                          used to drive playback, i.e. frames will be displayed in time with
                                          the playback of the media file.
    
            Returns:
                Canvas2D: a reference to the new Canvas object
        )scenepicdoc",
      "canvas_id"_a = "",
      "width"_a = 400,
      "height"_a = 400,
      "html_id"_a = "",
      "background_color"_a = Colors::Black,
      "media_id"_a = "")
    .def(
      "create_graph",
      &Scene::create_graph,
      R"scenepicdoc(
            Create a new Graph canvas and append to the Scene.

            Args:
                canvas_id (str, optional): a unique identifier for the Canvas (will be
                                           automatically populated if not provided). Defaults to None.
                width (int, optional): the width in pixels of the Canvas on the HTML page. Defaults to 400.
                height (int, optional): the height in pixels of the Canvas on the HTML page. Defaults to 400.
                name_align (str, optional): How to align the sparkline label
                                            (one of 'left', 'right', 'top', or 'bottom').
                                            Defaults to 'left'.
                value_align (str, optional): How to align the sparkline value
                                            (one of 'left', 'right', 'top', or 'bottom').
                                            Defaults to 'right'.
                html_id (str, optional): id of an HTML element to use as this
                                         Canvas's parent in the HTML DOM (otherwise simply
                                         appended to document). Defaults to None.
                background_color (np.ndarray, optional): the background color of the canvas. Defaults to White.
                margin (Margin, optional): the outer margin of the graph. Defaults to Margin(10).
                font_family (str, optional): the font family used for the graph labels. Defaults to "sans-serif".
                name_size (float, optional): the text size in pixels used for the graph labels. Defaults to 12.0.
                value_size (float, optional): the text size in pixels used for the graph values. Defaults to 12.0.
                media_id (str, optional): optional ID of a media file to attach to the canvas. This file will be
                                          used to drive playback, i.e. frames will be displayed in time with
                                          the playback of the media file.
    
            Returns:
                Graph: a reference to the new Graph object
        )scenepicdoc",
      "canvas_id"_a = "",
      "width"_a = 400,
      "height"_a = 400,
      "name_align"_a = "left",
      "value_align"_a = "right",
      "html_id"_a = "",
      "background_color"_a = Colors::White,
      "margin"_a = Graph::Margin(10),
      "font_family"_a = "sans-serif",
      "name_size"_a = 12.0,
      "value_size"_a = 12.0,
      "media_id"_a = "")
    .def(
      "create_mesh_",
      &Scene::create_mesh,
      "mesh_id"_a = "",
      "layer_id"_a = "",
      "double_sided"_a = false,
      "camera_space"_a = false,
      "shared_color"_a = Color::None(),
      "texture_id"_a = "",
      "nn_texture"_a = true,
      "use_texture_alpha"_a = false,
      "is_billboard"_a = false,
      "is_label"_a = false)
    .def(
      "update_mesh",
      &Scene::update_mesh,
      R"scenepicdoc(
            Create a new mesh from an existing one by replacing its vector positions, normals, and/or colors.

            Args:
                base_mesh_id (str): the unique identifier of the original base mesh. Must already have been created.
                positions (np.ndarray, optional): float32 matrix of [N, 3] new positions
                normals (np.ndarray, optional): float32 matrix of [N, 3] new normals.
                colors (np.ndarray, optional): float32 matrix of [N, 3] new colors.
                mesh_id (str, optional): a unique identifier for the new updated Mesh (will be
                                         automatically populated if not provided). Defaults to None.
            
            Returns:
                MeshUpdate: a reference to the MeshUpdate object
        )scenepicdoc",
      "base_mesh_id"_a,
      "positions"_a = VectorBufferNone(),
      "normals"_a = VectorBufferNone(),
      "colors"_a = VectorBufferNone(),
      "mesh_id"_a = "")
    .def(
      "update_instanced_mesh",
      &Scene::update_instanced_mesh,
      R"scenepicdoc(
            Create a new mesh from an existing one by replacing its vector positions and normals.

            Args:
                base_mesh_id (str): the unique identifier of the original base mesh. Must already have been created.
                positions (np.ndarray, optional): float32 matrix of [N, 3] new positions
                rotations (np.ndarray, optional): float32 matrix of [N, 4] new rotations.
                                                  Defaults to None, indicating no change to
                                                  the rotations.
                colors (np.ndarray, optional): float32 matrix of [N, 3] new colors
                mesh_id (str, optional): a unique identifier for the new updated Mesh (will be
                                         automatically populated if not provided). Defaults to None.
            
            Returns:
                MeshUpdate: a reference to the MeshUpdate object
        )scenepicdoc",
      "base_mesh_id"_a,
      "positions"_a = VectorBufferNone(),
      "rotations"_a = QuaternionBufferNone(),
      "colors"_a = ColorBufferNone(),
      "mesh_id"_a = "")
    .def(
      "update_mesh_without_normals",
      &Scene::update_mesh_without_normals,
      R"scenepicdoc(
            Deprecated.
        )scenepicdoc",
      "base_mesh_id"_a,
      "positions"_a,
      "mesh_id"_a = "")
    .def(
      "update_mesh_positions",
      &Scene::update_mesh_positions,
      R"scenepicdoc(
            Create a new mesh from an existing one by replacing its vector positions and compute normal values.

            Args:
                base_mesh_id (str): the unique identifier of the original base mesh. Must already have been created.
                positions (np.ndarray): float32 matrix of [N, 3] new positions
                mesh_id (str, optional): a unique identifier for the new updated Mesh (will be
                                         automatically populated if not provided). Defaults to None.
            
            Returns:
                MeshUpdate: a reference to the MeshUpdate object
        )scenepicdoc",
      "base_mesh_id"_a,
      "positions"_a,
      "mesh_id"_a = "")
    .def(
      "create_audio",
      &Scene::create_audio,
      R"scenepicdoc(
            Create a new AudioTrack object and append it to the Scene.
            Audio tracks are Scene-wide resources that can be attached to multiple canvases.

            Args:
                audio_id (str, optional): a unique identifier for the Audio track (will be
                                          automatically populated if not provided). Defaults to None.

            Returns:
                AudioTrack: a reference to an Audio track object
        )scenepicdoc",
      "audio_id"_a = "")
    .def(
      "create_video",
      &Scene::create_video,
      R"scenepicdoc(
            Create a new Video object and append it to the Scene.
            Videos are Scene-wide resources that can be attached to multiple canvases.

            Args:
                video_id (str, optional): a unique identifier for the Video (will be
                                          automatically populated if not provided). Defaults to None.

            Returns:
                Video: a reference to an Video object
        )scenepicdoc",
      "video_id"_a = "")
    .def(
      "create_image",
      &Scene::create_image,
      R"scenepicdoc(
            Create a new Image and append to the Scene.
            Images are Scene-wide resources that can be reused as textures
            across multiple Meshes and Frame2Ds.

            Args:
                image_id (str, optional): a unique identifier for the Image (will be
                                          automatically populated if not provided). Defaults to None.

            Returns:
                Image: a reference to the Image object
        )scenepicdoc",
      "image_id"_a = "")
    .def(
      "create_label",
      &Scene::create_label,
      R"scenepicdoc(
            Create a new Label and append to the Scene.
            Labels are Scene-wide resources that can be reused across
            multiple Meshes.

            Args:
                label_id (str, optional): a unique identifier for the Label (will be
                                          automatically populated if not provided). Defaults to None.
                text (str, optional): the text to use in the label. Defaults to "Text".
                color (np.ndarray, optional): the color to use. Defaults to White.
                size_in_pixels (float, optional): the size in pixels to display the label at. Defaults to 12.0.
                font_family (str, optional): HTML font family name. Defaults to "sans-serif".
                layer_id (str, optional): an identifier to allow several labels to be grouped together
                          into a "layer".  Layers should be used in conjunction with
                          Canvas3D.set_layer_settings(). Defaults to None.
                horizontal_align (str, optional): one of 'left', 'center', 'right'. how to align the
                                  label horizontally. Defaults to "left".
                vertical_align (str, optional): one of 'top', 'middle', 'bottom'. how to align the
                                label vertically. Defaults to "middle".
                offset_distance (float, optional): distance in world space to offset the label by when
                                 using left/right/top/bottom alignment. Defaults to 0.1.
                camera_space (bool, optional): whether the label should be created in
                                               camera space. Defaults to False.
            
            Returns:
                Label: a reference to the new Label object
        )scenepicdoc",
      "label_id"_a = "",
      "text"_a = "Text",
      "color"_a = Colors::White,
      "size_in_pixels"_a = 12.0,
      "font_family"_a = "sans-serif",
      "layer_id"_a = "",
      "horizontal_align"_a = "left",
      "vertical_align"_a = "middle",
      "offset_distance"_a = 0.1,
      "camera_space"_a = false)
    .def(
      "create_text_panel",
      &Scene::create_text_panel,
      R"scenepicdoc(
            Create a new TextPanel and add directly to the HTML page.

            Args:
                text_panel_id (str, optional): a unique identifier for the TextPanel (will be
                                               automatically populated if not provided). Defaults to None.
                html_id (str, optional): optional id of an HTML element to use as this
                                         TextPanel's parent in the HTML DOM (otherwise will
                                         simply be appended to document). Defaults to None.
                add_input_box (bool, optional): whether to add an input box to the TextPanel. Defaults to False.
            
            Returns:
                a reference to the new TextPanel object
        )scenepicdoc",
      "text_panel_id"_a = "",
      "html_id"_a = "",
      "add_input_box"_a = false)
    .def(
      "create_drop_down_menu",
      &Scene::create_drop_down_menu,
      R"scenepicdoc(
            Create a new DropDownMenu and add directly to the HTML page.

            Keyword Args:
                drop_down_menu_id (str, optional): a unique identifier for the DropDownMenu
                                                   (will be automatically populated if not
                                                   provided). Defaults to None.
                title (str, optional): a title for the drop down box. Defaults to None.
                html_id (str, optional): optional id of an HTML element to use as this
                                         TextPanel's parent in the HTML DOM (otherwise
                                         will simply be appended to document). Defaults to None.
            
            Returns:
                DropDownMenu: a reference to the new DropDownMenu object
        )scenepicdoc",
      "drop_down_menu_id"_a = "",
      "title"_a = "",
      "html_id"_a = "")
    .def(
      "quantize_updates",
      &Scene::quantize_updates,
      R"scenepicdoc(
            Quantize the mesh updates.

            Description:
                Each update will be reduced in size in such a way as to minimize the expected per-value error from quantization.
                The number of keyframes (and thus the level of compression) can be controlled via two
                thresholds: the relative error threshold, and the absolute error threshold. The relative
                threshold is evaluated as a fraction of max(positions) - min(positions). The absolute
                threshold is measured in raw units.
            
                If the relative error threshold has a positive value but the absolute error threshold is negative,
                only the relative threshold will be used. Conversely, if the relative threshold is negative and the
                absolute threshold is positive then only the absolute threshold will be used. If both are positive,
                then they will both be applied. If both are negative, an exception will be raised.
            
                The quantization process will complete in such a way that (quantized - actual).abs().max() < threshold.
                In essence, each x, y, and z coordinate will individually be different by no more than this threshold
                after quantization. Thus, a possible result of quantization will that every frame is a keyframe, i.e. no
                quantization occurs. More typically, however, the algorithm finds a few "keyframe" meshes which
                minimize the expected error across the remaining (quantized) meshes.            

            Args:
                relative_error_threshold (float, optional): the maximum expected error as a multiple of the range of
                                                            values in the base mesh. Defaults to 1e-5.
                absolute_error_threshold (float, optional): the maximum expected error in absolute units. Defaults to -1.0.
                base_mesh_id (str, optional): ID of the base mesh to use as a filter on quantization. Defaults to None.
                per_frame_range (bool, optional): Whether to use the most accurate range per frame, increasing accuracy
                                                  but reducing compression. Defaults to True.

            Returns:
                Mapping[str, QuantizationInfo]: information on the per-mesh quantization process
        )scenepicdoc",
      "relative_error_threshold"_a = 1e-5,
      "absolute_error_threshold"_a = -1.0,
      "base_mesh_id"_a = "",
      "per_frame_range"_a = true)
    .def("measure_command_size", &Scene::measure_command_size, R"scenepicdoc(
            Measures the number of bytes used by command type.

            Return:
                Mapping[str, int]: the number of bytes per command type
        )scenepicdoc")
    .def_property(
      "framerate",
      py::overload_cast<>(&Scene::framerate, py::const_),
      py::overload_cast<float>(&Scene::framerate),
      R"scenepicdoc(
                          float: Number of frames to display per second
                      )scenepicdoc")
    .def_property(
      "status_bar_visibility",
      py::overload_cast<>(&Scene::status_bar_visibility, py::const_),
      py::overload_cast<const std::string&>(&Scene::status_bar_visibility),
      R"scenepicdoc(
                          str: CSS visibility for the status bar
                      )scenepicdoc")
    .def(
      "configure_user_interface",
      &Scene::configure_user_interface,
      R"scenepicdoc(
            Set user interface parameters across all Canvases with given
            UIParameters instance.

            Args:
                ui_parameters (UIParameters): global UI parameters to set
        )scenepicdoc",
      "ui_parameters"_a)
    .def("link_canvas_events_", &Scene::link_canvas_events, "canvas_ids"_a)
    .def(
      "simulate_key_presses_",
      &Scene::simulate_key_presses,
      "canvas_id"_a,
      "keypresses"_a)
    .def("clear_script", &Scene::clear_script, R"scenepicdoc(
            Call this if you are dumping a ScenePic script in multiple parts,
            e.g. for client-server interactions, or when chunking a large
            visualization into multiple files. Immediately after this, a
            call to get_script() will returnempty.
        )scenepicdoc")
    .def("get_json", &Scene::json, R"scenepicdoc(
            Returns a JSON-serialized representation of the Scene as a string.

            If you use clear_script() then this will return updates to
            the Scene since thelast call to clear_script().

            Returns:
                str: a JSON string representing the Scene
        )scenepicdoc")
    .def("get_script", &Scene::script, R"scenepicdoc(
            Returns a JSONP (https://en.wikipedia.org/wiki/JSONP) script
            representing the Scene.
            If you use clear_script() then this will return updates to
            the Scene since the last call to clear_script().

            Returns:
                str: a JSONP script representing the Scene
        )scenepicdoc")
    .def(
      "save_as_json",
      &Scene::save_as_json,
      R"scenepicdoc(
            To view the JSON, you will need to separately code up the
            wrapper html and provide the scenepic.min.js library file.
            Alternatively, use save_as_html() to make a self-contained
            HTML file.

            Args:
                path (str): the path to the file on disk
        )scenepicdoc",
      "path"_a)
    .def(
      "save_as_script",
      &Scene::save_as_script,
      R"scenepicdoc(
            Save the scene as a ScenePic script file (JavaScript JSONP
            format).
            To view the script, you will need to separately code up
            the wrapper html.
            Alternatively, use save_as_html() to make a self-contained
            HTML file.

            Args:
                path (str): the path to the file on disk
                standalone (bool): whether to make the script standalone
                                   by including the library
        )scenepicdoc",
      "path"_a,
      "standalone"_a = false)
    .def(
      "save_as_html",
      &Scene::save_as_html,
      R"scenepicdoc(
            Save the scene as a self-contained html file with no
            dependencies.

            Args:
                path (str): the path to the file on disk
                title (str, optional): the HTML title. Defaults to "ScenePic".
                head_html (str, optional): the raw HTML to place in the HEAD tag. Defaults to None.
                body_html (str, optional): the raw HTML to place in the BODY tag. Defaults to None.
                script_path (str, optional): desired relative path for the script. A value of None
                                             indicates to embed the script in the HTML page.
                                             Defaults to None.          
                library_path (str, optional): desired relative path for the library. A value of None
                                              indicates to embed the library in the HTML page.
                                              Defaults to None.          
        )scenepicdoc",
      "path"_a,
      "title"_a = "ScenePic ",
      "head_html"_a = "",
      "body_html"_a = "",
      "script_path"_a = "",
      "library_path"_a = "")
    .def_property_readonly(
      "script_cleared",
      &Scene::script_cleared,
      "bool: Whether the script has been cleared");
}