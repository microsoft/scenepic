"""Set of unit tests formed from the Python Tutorial example."""

import json
import math
import random

import numpy as np
import svt


def test_scene_and_canvas_basics():
    """Test the most basic functionality."""
    scene = svt.Scene()
    scene.create_canvas_3d(width=300, height=300)
    scene.create_canvas_3d(width=100, height=300)


def test_meshes_and_frames():
    """Test creating meshes and frames."""
    scene = svt.Scene()

    my_first_mesh = scene.create_mesh(shared_color=svt.Color(1.0, 0.0, 1.0))
    my_first_mesh.add_cube(transform=svt.Transforms.Scale(0.1))
    my_first_mesh.add_cube(transform=np.dot(svt.Transforms.Translate(
        [-1.0, 1.0, -1.0]), svt.Transforms.Scale(0.5)))
    my_first_mesh.add_sphere(
        transform=svt.Transforms.Translate([1.0, 1.0, 1.0]))

    canvas = scene.create_canvas_3d(width=300, height=300)

    for i in range(10):
        frame = canvas.create_frame()
        frame.add_mesh(my_first_mesh, transform=svt.Transforms.Translate(
            [i / 10.0, 0.0, 0.0]))
        mesh2 = scene.create_mesh(shared_color=svt.Color(
            1.0, 0.0, 0.0), camera_space=True)
        mesh2.add_cube(transform=np.dot(svt.Transforms.Translate(
            [0.0, 0.0, -5.0]), svt.Transforms.Scale(0.5)))
        frame.add_mesh(mesh2)
        label = scene.create_label(text="Hi", color=svt.Colors.White,
                                   size_in_pixels=80, offset_distance=0.6, camera_space=True)
        frame.add_label(label=label, position=[0.0, 0.0, -5.0])


def test_point_clouds_1():
    """Test creating point clouds."""
    scene = svt.Scene()

    mesh = scene.create_mesh(shared_color=svt.Color(0, 1, 0))
    mesh.add_cube()
    mesh.apply_transform(svt.Transforms.Scale(0.01))
    mesh.enable_instancing(positions=2 * np.random.rand(10000, 3) - 1)

    canvas = scene.create_canvas_3d(
        width=300, height=300, shading=svt.Shading(bg_color=svt.Colors.White))
    frame = canvas.create_frame()
    frame.add_mesh(mesh)


def test_point_clouds_2():
    """More testing of point clouds."""
    disc_thickness = 0.2
    normal_length = 1.5
    point_size = 0.1

    sphere_mesh = svt.Mesh()
    sphere_mesh.add_sphere(transform=svt.Transforms.Scale(2.0), color=svt.Color(1.0, 0.0, 0.0))
    num_vertices = sphere_mesh.count_vertices()
    points = sphere_mesh.vertex_buffer["pos"]
    normals = sphere_mesh.vertex_buffer["norm"]

    rotations = np.zeros((num_vertices, 4))
    for i in range(0, num_vertices):
        rotations[i, :] = svt.Transforms.QuaternionToRotateXAxisToAlignWithAxis(
            normals[i, :])

    colors = np.random.rand(num_vertices, 3)

    scene = svt.Scene()

    mesh = scene.create_mesh(shared_color=svt.Color(0, 1, 0), double_sided=True)

    mesh.add_disc(segment_count=20, transform=svt.Transforms.Scale(
        [disc_thickness, 1.0, 1.0]))
    mesh.add_thickline(start_point=np.array([disc_thickness * 0.5, 0.0, 0.0]),
                       end_point=np.array([normal_length, 0.0, 0.0]),
                       start_thickness=0.2,
                       end_thickness=0.1)
    mesh.apply_transform(svt.Transforms.Scale(point_size))

    mesh.enable_instancing(positions=points, rotations=rotations, colors=colors)

    canvas = scene.create_canvas_3d(width=300, height=300)
    frame = canvas.create_frame()
    frame.add_mesh(mesh)


def test_misc_meshes():
    """Test various miscellaneous meshes."""
    scene = svt.Scene()

    mesh1 = scene.create_mesh(layer_id="Sphere+")
    mesh1.add_cube(color=svt.Color(1.0, 0.0, 0.0),
                   transform=svt.Transforms.Translate([-2.0, 0.0, -2.0]))
    mesh1.add_uv_sphere(color=svt.Color(0.0, 0.0, 1.0),
                        transform=np.dot(svt.Transforms.Translate([-1.0, 1.0, 0.0]),
                                         svt.Transforms.Scale(1.8)),
                        fill_triangles=False,
                        add_wireframe=True)
    mesh1.add_icosphere(color=svt.Color(0.0, 1.0, 1.0),
                        transform=np.dot(svt.Transforms.Translate([2.0, 1.0, 0.0]),
                                         svt.Transforms.Scale(1.8)),
                        fill_triangles=False,
                        add_wireframe=True,
                        steps=2)

    mesh2 = scene.create_mesh(layer_id="Coords")
    mesh2.add_coordinate_axes(transform=svt.Transforms.Translate([0.0, 0.0, 0.0]))

    cube_verts = np.array([[-0.5, -0.5, -0.5],
                           [+0.5, -0.5, -0.5],
                           [-0.5, +0.5, -0.5],
                           [+0.5, +0.5, -0.5],
                           [-0.5, -0.5, +0.5],
                           [+0.5, -0.5, +0.5],
                           [-0.5, +0.5, +0.5],
                           [+0.5, +0.5, +0.5]])
    cube_tris = np.array([[0, 2, 3],
                          [0, 3, 1],
                          [1, 3, 7],
                          [1, 7, 5],
                          [4, 5, 7],
                          [4, 7, 6],
                          [4, 6, 2],
                          [4, 2, 0],
                          [2, 6, 7],
                          [2, 7, 3],
                          [4, 0, 1],
                          [4, 1, 5]])
    cube_verts_a, cube_tris_a = svt.LoopSubdivStencil(cube_tris, 2, False).apply(cube_verts)
    cube_verts_b, cube_tris_b = svt.LoopSubdivStencil(cube_tris, 2, True).apply(cube_verts)
    mesh3 = scene.create_mesh(shared_color=svt.Color(1.0, 0.8, 0.8))
    mesh3.add_mesh_without_normals(cube_verts, cube_tris,
                                   transform=svt.Transforms.Translate([-1.0, 0.0, 0.0]))
    mesh3.add_mesh_without_normals(cube_verts_a, cube_tris_a)
    mesh3.add_mesh_without_normals(cube_verts_b, cube_tris_b,
                                   transform=svt.Transforms.Translate([+1.0, 0.0, 0.0]))

    mesh4 = scene.create_mesh()
    num_segs = 7000
    positions = np.cumsum(np.random.rand(num_segs, 3) * 0.2, axis=0)
    colored_points = np.concatenate(
        (positions, np.random.rand(num_segs, 3)), axis=1)
    mesh4.add_lines(colored_points[0:-1, :], colored_points[1:, :])
    mesh4.add_camera_frustum(color=svt.Color(1.0, 1.0, 0.0))

    canvas1 = scene.create_canvas_3d(width=300, height=300)
    canvas2 = scene.create_canvas_3d(width=300, height=300)

    scene.link_canvas_events(canvas1, canvas2)

    canvas1.set_layer_settings({
        "Coords": {"visible": False},
        "Sphere+": {"visible": True}
    })

    canvas1.create_frame(meshes=[mesh1, mesh2])
    canvas2.create_frame(meshes=[mesh2, mesh3])
    canvas2.create_frame(meshes=[mesh4, mesh1])


def test_images_and_textures(asset):
    """Test loading images."""
    scene = svt.Scene()

    image1 = scene.create_image(image_id="PolarBear")
    image1.load(asset("PolarBear.png"))

    texture = scene.create_image(image_id="texture")
    texture.load(asset("uv.png"))

    cam_space_mesh = scene.create_mesh(shared_color=svt.Color(1.0, 0.0, 0.0), camera_space=True)
    cam_space_mesh.add_sphere(transform=np.dot(svt.Transforms.Translate([10, -10, -20.0]),
                                               svt.Transforms.Scale(1.0)))

    sphere = scene.create_mesh(texture_id=texture.image_id, nn_texture=False)
    sphere.add_icosphere(steps=4)

    cube = scene.create_mesh(texture_id=texture.image_id)
    transform = svt.Transforms.translate([-1, 0, 0]) @ svt.Transforms.scale(0.5)
    cube.add_cube(transform=transform)

    canvas = scene.create_canvas_3d(shading=svt.Shading(bg_color=svt.Colors.White))
    mesh1 = scene.create_mesh(texture_id="PolarBear")
    mesh1.add_image()

    n_frames = 20
    for i in range(n_frames):
        angle = 2 * math.pi * i / n_frames
        cos, sin = math.cos(angle), math.sin(angle)

        axis = np.array([1.0, 0.0, 1.0])
        axis /= np.linalg.norm(axis)
        focus_point = svt.FocusPoint([cos, sin, 0], orientation_axis_angle=axis * angle)

        mesh = scene.create_mesh()
        mesh.add_coordinate_axes(transform=np.dot(
            svt.Transforms.Translate(focus_point.position),
            svt.Transforms.RotationMatrixFromAxisAngle(axis, angle)))

        im_size = 15
        im_data = np.random.rand(im_size, im_size, 4)
        im_data[:, :, 3] = 0.5 + 0.5 * im_data[:, :, 3]

        image_b = scene.create_image()
        image_b.from_numpy(im_data)
        mesh_b = scene.create_mesh(texture_id=image_b, is_billboard=True, use_texture_alpha=True)
        mesh_b.add_image(transform=np.dot(svt.Transforms.Scale(2.0),
                                          svt.Transforms.Translate([0, 0, -1])))

        frame = canvas.create_frame(focus_point=focus_point)
        frame.add_mesh(mesh1, transform=svt.Transforms.Translate([cos, sin, 0]))
        frame.add_mesh(mesh_b, transform=np.dot(svt.Transforms.Scale(i * 1.0 / n_frames),
                                                svt.Transforms.Translate([-cos, -sin, 0])))
        frame.add_mesh(cam_space_mesh)
        frame.add_mesh(sphere, transform=svt.Transforms.rotation_about_y(np.pi * 2 * i / n_frames))
        frame.add_mesh(cube, transform=svt.Transforms.rotation_about_y(-np.pi * 2 * i / n_frames))
        frame.add_mesh(mesh)


def test_2d_canvases(asset):
    """Tests for 2D canvases."""
    scene = svt.Scene()

    image1 = scene.create_image(image_id="PolarBear")
    image1.load(asset("PolarBear.png"))

    image2 = scene.create_image(image_id="Random")
    image2.from_numpy(np.random.rand(20, 30, 3) * 128 / 255.0)

    canvas1 = scene.create_canvas_2d(width=400, height=300, background_color=svt.Colors.White)
    canvas1.create_frame().add_image(image1, "fit")
    canvas1.create_frame().add_image(image1, "fill")
    canvas1.create_frame().add_image(image1, "stretch")
    canvas1.create_frame().add_image(image1, "manual", x=50, y=50, scale=0.3)

    canvas2 = scene.create_canvas_2d(width=300, height=300)
    frame = canvas2.create_frame()
    frame.add_image(image2, "fit")
    frame.add_image(image1, "manual", x=30, y=30, scale=0.2)
    frame.add_circle(200, 200, 40, fill_color=svt.Colors.Black,
                     line_width=10, line_color=svt.Colors.Blue)
    frame.add_rectangle(200, 100, 50, 25,
                        fill_color=svt.Colors.Green, line_width=0)
    frame.add_text("Hello World", 30, 100, svt.Colors.White, 100, "segoe ui light")


def test_opacity_and_labels():
    """Test opacity and label functionality."""
    random.seed(55)

    scene = svt.Scene()
    canvas = scene.create_canvas_3d(width=700, height=700)
    frame = canvas.create_frame()

    layer_settings = {"Labels": {"opacity": 1.0}}
    num_objects = 20
    for i in range(num_objects):
        geotype = random.randint(0, 1)
        color = svt.Color(random.random(), random.random(), random.random())
        size = 0.3 * random.random() + 0.2
        position = 3.0 * np.array([random.random(), random.random(), random.random()]) - 1.5
        opacity = 1.0 if random.randint(0, 1) == 0 else random.uniform(0.45, 0.55)

        layer_id = "Layer" + str(i)
        mesh = scene.create_mesh(shared_color=color, layer_id=layer_id)
        layer_settings[layer_id] = {"opacity": opacity}
        if geotype == 0:
            mesh.add_cube()
        elif geotype == 1:
            mesh.add_sphere()
        mesh.apply_transform(svt.Transforms.Scale(size))
        mesh.apply_transform(svt.Transforms.Translate(position))
        frame.add_mesh(mesh)

        text = "{0:0.2f} {1:0.2f} {2:0.2f} {3:0.2f}".format(
            color[0], color[1], color[2], opacity)
        horizontal_align = ["left", "center", "right"][random.randint(0, 2)]
        vertical_align = ["top", "middle", "bottom"][random.randint(0, 2)]
        if geotype == 0:
            if horizontal_align != "center" and vertical_align != "middle":
                offset_distance = size * 0.7
            else:
                offset_distance = size * 0.9
        else:
            if horizontal_align != "center" and vertical_align != "middle":
                offset_distance = size * 0.5 * 0.8
            else:
                offset_distance = size * 0.6
        label = scene.create_label(text=text,
                                   color=color,
                                   layer_id="Labels",
                                   font_family="consolas",
                                   size_in_pixels=80 * size,
                                   offset_distance=offset_distance,
                                   vertical_align=vertical_align,
                                   horizontal_align=horizontal_align)
        frame.add_label(label=label, position=position)

    canvas.set_layer_settings(layer_settings)


def test_animation(asset):
    scene = svt.Scene()
    canvas = scene.create_canvas_3d(width=700, height=700)

    jelly_mesh = svt.load_obj(asset("jelly.obj"))
    texture = scene.create_image("texture")
    texture.load(asset("jelly.png"))

    base_mesh = scene.create_mesh("jelly_base")
    base_mesh.texture_id = texture.image_id
    base_mesh.use_texture_alpha = True
    base_mesh.add_mesh(jelly_mesh)

    marble = scene.create_mesh("plate", shared_color=svt.Colors.White)
    marble.add_sphere(transform=svt.Transforms.Scale(0.4))

    for i in range(60):
        frame = canvas.create_frame()
        positions = jelly_mesh.positions.copy()
        delta_x = (positions[:, 0] + 0.0838 * i) * 10
        delta_z = (positions[:, 2] + 0.0419 * i) * 10
        positions[:, 1] = positions[:, 1] + 0.1 * (np.cos(delta_x) + np.sin(delta_z))

        mesh_update = scene.update_mesh("jelly_base", positions)
        frame.add_mesh(mesh_update)

        marble_y = np.sin(0.105 * i)
        frame.add_mesh(marble, transform=svt.Transforms.Translate([0, marble_y, 0]))

    scene.quantize_updates()


def test_camera_movement(asset):
    scene = svt.Scene()
    spin_canvas = scene.create_canvas_3d("spin")
    spiral_canvas = scene.create_canvas_3d("spiral")

    polar_bear = scene.create_image(image_id="polar_bear")
    polar_bear.load(asset("PolarBear.png"))
    uv_texture = scene.create_image(image_id="texture")
    uv_texture.load(asset("uv.png"))

    cube = scene.create_mesh("cube", texture_id=polar_bear.image_id)
    cube.add_cube()
    sphere = scene.create_mesh("sphere", texture_id=uv_texture.image_id)
    sphere.add_icosphere(steps=4, transform=svt.Transforms.translate([0, 1, 0]))

    spin_frustum = scene.create_mesh("spin_frustum", shared_color=svt.Colors.Red)
    spiral_frustum = scene.create_mesh("spiral_frustum", shared_color=svt.Colors.Green)

    spin_frustum.add_camera_frustum(depth=-1, transform=svt.Transforms.rotation_about_z(np.pi))
    spiral_frustum.add_camera_frustum(depth=-1, transform=svt.Transforms.rotation_about_z(np.pi))

    num_frames = 60
    for i in range(num_frames):
        spin_frame = spin_canvas.create_frame()
        spin_frame.add_meshes([cube, sphere])

        angle = i*np.pi*2/num_frames

        rotation = svt.Transforms.rotation_about_z(angle)
        spin_frame.camera = svt.Camera(center=[0, 0, 4], rotation=rotation, fov_y_degrees=30.0)

        spiral_frame = spiral_canvas.create_frame()
        spiral_frame.add_meshes([cube, sphere])

        camera_center = [4*np.cos(angle), i*4/num_frames - 2, 4*np.sin(angle)]
        spiral_frame.camera = svt.Camera(camera_center, look_at=[0, 0.5, 0])

        spiral_frame.add_mesh(spiral_frustum, transform=spiral_frame.camera.camera_to_world)
        spiral_frame.add_mesh(spin_frustum, transform=spin_frame.camera.camera_to_world)
        spin_frame.add_mesh(spiral_frustum, transform=spiral_frame.camera.camera_to_world)
        spin_frame.add_mesh(spin_frustum, transform=spin_frame.camera.camera_to_world)

    scene.link_canvas_events(spin_canvas, spiral_canvas)


def _set_audio(scene, canvas, path):
    audio = scene.create_audio()
    audio.load(path)
    canvas.media_id = audio.audio_id


def test_audio_tracks(asset):
    scene = svt.Scene()

    names = ["red", "green", "blue"]
    colors = [svt.Colors.Red, svt.Colors.Green, svt.Colors.Blue]
    frequencies = [0, 1, 0.5]

    graph = scene.create_graph("graph", width=900, height=150)
    for name, color, frequency in zip(names, colors, frequencies):
        mesh = scene.create_mesh()
        mesh.add_cube(color)
        canvas = scene.create_canvas_3d(name, width=300, height=300)
        _set_audio(scene, canvas, asset(name + ".ogg"))
        values = []

        for j in range(60):
            frame = canvas.create_frame()
            scale = math.sin(j * 2 * math.pi * frequency / 30)
            frame.add_mesh(mesh, svt.Transforms.scale((scale + 1) / 2 + 0.5))
            values.append(scale)

        graph.add_sparkline(name, values, color)

    names.append("graph")
    scene.grid("600px", "1fr auto", "1fr 1fr 1fr")
    scene.place("graph", "2", "1 / span 3")
    scene.link_canvas_events(*names)


def _angle_to_pos(angle, radius):
    return np.cos(angle) * radius + 200, np.sin(angle) * radius + 200


def test_circles_video(asset):
    scene = svt.Scene()

    video = scene.create_video()
    video.load(asset("circles.mp4"))

    tracking = scene.create_canvas_2d("tracking", background_color=svt.Colors.White)
    tracking.media_id = video.video_id

    multi = scene.create_canvas_2d("multi", background_color=svt.Colors.White)
    multi.media_id = video.video_id

    angles = np.linspace(0, 2 * np.pi, 360, endpoint=False)
    for angle in angles:
        frame = tracking.create_frame()
        frame.add_video(layer_id="video")

        red_pos = _angle_to_pos(angle, 160)
        frame.add_rectangle(red_pos[0] - 11, red_pos[1] - 11, 22, 22, [255, 0, 0], 2, layer_id="rect")
        frame.add_circle(red_pos[0], red_pos[1], 10, fill_color=[255, 0, 0], layer_id="dot")

        green_pos = _angle_to_pos(-2*angle, 80)
        frame.add_rectangle(green_pos[0] - 11, green_pos[1] - 11, 22, 22, [0, 255, 0], 2, layer_id="rect")
        frame.add_circle(green_pos[0], green_pos[1], 10, fill_color=[0, 255, 0], layer_id="dot")

        blue_pos = _angle_to_pos(4*angle, 40)
        frame.add_rectangle(blue_pos[0] - 11, blue_pos[1] - 11, 22, 22, [0, 0, 255], 2, layer_id="rect")
        frame.add_circle(blue_pos[0], blue_pos[1], 10, fill_color=[0, 0, 255], layer_id="dot")

        frame = multi.create_frame()
        frame.add_video("manual", red_pos[0] - 40, red_pos[1] - 40, 0.2, layer_id="red")
        frame.add_video("manual", green_pos[0] - 25, green_pos[1] - 25, 0.125, layer_id="green")
        frame.add_video("manual", 160, 160, 0.2, layer_id="blue")

    tracking.set_layer_settings({
        "rect": {"render_order": 0},
        "video": {"render_order": 1},
        "dot": {"render_order": 2}
    })

    multi.set_layer_settings({
        "red": {},
        "green": {},
        "blue": {}
    })

    scene.link_canvas_events("tracking", "multi")


def _load_camera(camera_info):
    location = np.array(camera_info["location"], np.float32)
    euler_angles = np.array(camera_info["rotation"], np.float32)
    rotation = svt.Transforms.euler_angles_to_matrix(euler_angles, "XYZ")
    translation = svt.Transforms.translate(location)
    extrinsics = translation @ rotation
    world_to_camera = svt.Transforms.gl_world_to_camera(extrinsics)
    aspect_ratio = camera_info["width"] / camera_info["height"]
    projection = svt.Transforms.gl_projection(camera_info["fov"], aspect_ratio, 0.01, 100)

    return svt.Camera(world_to_camera, projection)


def _load_cameras(path):
    with open(path) as file:
        cameras = json.load(file)
        return [_load_camera(cameras[key])
                for key in cameras]


def test_multiview(asset):
    scene = svt.Scene()

    cameras = _load_cameras(asset("cameras.json"))

    texture = scene.create_image("texture")
    texture.load(asset("PolarBear.png"))
    cube = scene.create_mesh("cube")
    cube.texture_id = texture.image_id
    cube.add_cube(transform=svt.Transforms.scale(2))

    frustums = scene.create_mesh("frustums", layer_id="frustums")
    colors = [svt.Colors.Red, svt.Colors.Green, svt.Colors.Blue]
    paths = [asset(name) for name in ["render0.png", "render1.png", "render2.png"]]
    camera_images = []
    images = []

    for i, (color, path, camera) in enumerate(zip(colors, paths, cameras)):
        image = scene.create_image(path)
        image.load(path)
        frustums.add_camera_frustum(camera, color)

        image_mesh = scene.create_mesh("image{}".format(i),
                                       layer_id="images",
                                       shared_color=svt.Colors.Gray,
                                       double_sided=True,
                                       texture_id=image.image_id)
        image_mesh.add_camera_image(camera)

        images.append(image)
        camera_images.append(image_mesh)

    width = 640
    for i, camera in enumerate(cameras):
        height = width / camera.aspect_ratio
        canvas = scene.create_canvas_3d("hand{}".format(i), width, height, camera=camera)
        frame = canvas.create_frame()
        frame.add_mesh(cube)
        frame.add_mesh(frustums)
        frame.camera = camera
        for cam_mesh in camera_images:
            frame.add_mesh(cam_mesh)
