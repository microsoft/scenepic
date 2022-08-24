import os

import numpy as np
import scenepic as sp


def _asset(name: str):
    asset_dir = os.path.join(os.path.dirname(__file__), "..", "ci", "assets")
    return os.path.join(asset_dir, name)


def _get_bounds(verts: np.ndarray) -> np.ndarray:
    vert_max = verts.max(0)
    vert_min = verts.min(0)
    return 0.5 * (vert_min + vert_max), vert_max - vert_min


class Animation:
    """This class will create the animation during its initialization.

    This is one of many potential patterns to use when constructing more complex
    visualizations. The main advantage is that you can pass all configuration
    information into the constructor, and then share variables between
    different stages of the visualization or animation. This animation
    has three stages:

    1. Point cloud converging to four cubes
    2. Four cubes morphing into the ScenePic logo
    3. Still shot on the logo

    Each is constructed in a separate method, but using shared elements.
    One element used in this particular animation is a pre-computed camera
    track. While the user can still move the camera around freely,
    this can be helpful for create videos from scenepic (e.g. the animated
    GIF in the README.)
    """

    def __init__(self,
                 num_cloud_frames=30,
                 num_morph_frames=60,
                 num_still_frames=30,
                 start_distance=0.1,
                 end_distance=2,
                 start_angles=(1.5, 1, -0.1),
                 end_angles=(-0.8, -0.2, 0),
                 width=1280,
                 height=640,
                 num_points=1000):
        """Constructor.

        Args:
            scene (sp.Scene): The ScenePic scene.
            num_cloud_frames (int, optional): Length of cloud stage. Defaults to 30.
            num_morph_frames (int, optional): Length of morph stage. Defaults to 60.
            num_still_frames (int, optional): Length of still stage. Defaults to 30.
            start_distance (float, optional): Starting camera distance. Defaults to 0.1.
            end_distance (int, optional): Ending camera distance. Defaults to 2.
            start_angles (tuple, optional): Starting camera angles. Defaults to (1.5, 1, -0.1).
            end_angles (tuple, optional): Ending camera angles. Defaults to (-0.8, -0.2, 0).
            width (int, optional): Width of the viewport. Defaults to 1280.
            height (int, optional): Height of the viewport. Defaults to 640.
            num_points (int, optional): Size of the point cloud. Defaults to 1000.
        """
        self.num_cloud_frames = num_cloud_frames
        self.num_morph_frames = num_morph_frames
        self.num_still_frames = num_still_frames
        self.num_frames = num_cloud_frames + num_morph_frames + num_still_frames
        self.aspect_ratio = width / height

        self.colors = [sp.ColorFromBytes(242, 80, 34),
                       sp.ColorFromBytes(127, 186, 0),
                       sp.ColorFromBytes(0, 164, 239),
                       sp.ColorFromBytes(255, 185, 0)]

        self.scene = sp.Scene()
        self.canvas = self.scene.create_canvas_3d(width=width, height=height)

        # load the text meshes
        scene_obj = sp.load_obj(_asset("scene.obj"))
        self.scene_mesh = self.scene.create_mesh(shared_color=self.colors[0], layer_id="text")
        self.scene_mesh.add_mesh(scene_obj)
        self.scene_mesh.apply_transform(sp.Transforms.rotation_about_x(np.pi / 2))
        self.scene_pos = [0, 0, 0]

        pic_obj = sp.load_obj(_asset("pic.obj"))
        self.pic_mesh = self.scene.create_mesh(shared_color=self.colors[3], layer_id="text")
        self.pic_mesh.add_mesh(pic_obj)
        self.pic_pos = [1.25, -0.05, 0.5]

        # create the cubes for morphing
        self.cubes = [self._create_cube(color) for color in self.colors]
        self.cube1_pos = [2, 0.15, 0.25]
        self.cube2_pos = [0.4, -0.05, 0.3]
        self.cube2_scale = [1.5, 0.2, 0.8]
        self.focus_point = [1, 0.25, 0.5]

        # compute the angles and distances for the cameras
        # the camera is parametrized as a "look-at" camera,
        # with a Z-distance from the focus point that is rotated
        # using the provided angles to get a center, and
        # then focused on a set point.
        self.angles, self.distances = self._compute_camera_info(start_distance,
                                                                end_distance,
                                                                start_angles,
                                                                end_angles)

        self.index = 0
        self._animate_cloud(num_points)
        self._animate_morph()
        self._animate_still()

    def save(self, path: str):
        """Save the animation to the specified path.

        Args:
            path (str): path to the HTML output file
        """
        self.scene.save_as_html(path, "Hello Scenepic!")

    def _compute_camera_info(self,
                             start_distance: np.ndarray,
                             end_distance: np.ndarray,
                             start_angles: np.ndarray,
                             end_angles: np.ndarray):
        stops = [self.num_cloud_frames,
                 self.num_cloud_frames + self.num_morph_frames,
                 self.num_frames - self.num_still_frames]

        angles = np.zeros((self.num_frames, 3), np.float32)
        angles[:stops[0]] = np.linspace(start_angles, [0, 0, 0], self.num_cloud_frames)
        angles[stops[0]:stops[1]] = np.linspace([0, 0, 0], end_angles, self.num_morph_frames)
        angles[stops[1]:] = end_angles

        distances = np.zeros(self.num_frames, np.float32)
        distances[:stops[0]] = np.linspace(start_distance, end_distance, self.num_cloud_frames)
        distances[stops[0]:] = end_distance

        return angles, distances

    def _create_camera(self, focus_point: np.ndarray) -> sp.Camera:
        # compute the rotation matrix
        angles = self.angles[self.index]
        rotation = sp.Transforms.euler_angles_to_matrix(angles)

        # create a vector at the current distance
        pos = np.array([0, 0, self.distances[self.index]], np.float32)

        # rotate and offset from the focus point
        pos = rotation[:3, :3] @ pos
        pos += focus_point

        # NB each call to this function will move the camera long its track
        self.index += 1

        return sp.Camera(pos, look_at=focus_point, aspect_ratio=self.aspect_ratio)

    def _animate_cloud(self, num_points: int):
        num_frames = self.num_cloud_frames

        # start with random positions
        start_positions = np.random.uniform(-1, 1, (num_points, 3)).astype(np.float32)
        end_positions = np.zeros_like(start_positions)

        # group the points by which will go to each cube
        groups = [num_points // 4, num_points // 2, 3 * num_points // 4]

        # distribute the final points randomly within each target cube
        end_positions[:groups[0]] = np.random.uniform([-0.51, 0.03, -0.22],
                                                      [-0.03, 0.51, 0.22],
                                                      (groups[0], 3))
        end_positions[groups[0]:groups[1]] = np.random.uniform([0.03, 0.03, -0.22],
                                                               [0.51, 0.51, 0.22],
                                                               (groups[1] - groups[0], 3))
        end_positions[groups[1]:groups[2]] = np.random.uniform([-0.51, -0.51, -0.22],
                                                               [-0.03, -0.03, 0.22],
                                                               (groups[2] - groups[1], 3))
        end_positions[groups[2]:] = np.random.uniform([0.03, -0.51, -0.22],
                                                      [0.51, -0.03, 0.22],
                                                      (num_points - groups[2], 3))

        # set the cloud colors to correspond to the target cubes
        colors = np.zeros_like(start_positions)
        colors[:groups[0]] = self.colors[0]
        colors[groups[0]:groups[1]] = self.colors[1]
        colors[groups[1]:groups[2]] = self.colors[2]
        colors[groups[2]:] = self.colors[3]

        positions = np.linspace(start_positions, end_positions, num_frames)

        for frame_positions in positions:
            mesh = self.scene.create_mesh(shared_color=sp.Colors.White, layer_id="cloud")
            mesh.add_cube(transform=sp.Transforms.scale(0.05))
            mesh.enable_instancing(frame_positions, colors=colors)

            frame = self.canvas.create_frame()
            frame.add_mesh(mesh)
            frame.camera = self._create_camera([0, 0, 0])

    def _animate_morph(self):
        num_frames = self.num_morph_frames

        # because the text will be hiding within the cubes
        # and then eventually revealed, we need to compute
        # some bounds information to position it correctly

        scene_pos, scene_scale = _get_bounds(self.scene_mesh.vertex_buffer["pos"])
        scene_positions = np.linspace([-0.47, 0.07, 0], self.scene_pos, num_frames)
        scene_scales = np.linspace(0.4 / scene_scale, [1, 1, 1], num_frames)
        scene_scales[:, 2] = 1

        _, pic_scale = _get_bounds(self.pic_mesh.vertex_buffer["pos"])
        pic_positions = np.linspace([0.27, -0.27, pic_scale[2] * 0.5], self.pic_pos, num_frames)
        pic_scales = np.linspace(0.4 / pic_scale, [1, 1, 1], num_frames)
        pic_scales[:, 1] = 1
        pic_pos = self.pic_pos.copy()
        pic_pos[2] -= 0.5 * pic_scale[2]

        # create the animated positions and scales for the cubes
        positions = np.stack([np.linspace([-0.27, 0.27, 0], scene_pos, num_frames),
                              np.linspace([0.27, 0.27, 0], self.cube1_pos, num_frames),
                              np.linspace([-0.27, -0.27, 0], self.cube2_pos, num_frames),
                              np.linspace([0.27, -0.27, 0], pic_pos, num_frames)])

        scales = np.stack([np.linspace([1, 1, 1], scene_scale * 2, num_frames),
                           np.linspace([1, 1, 1], [1, 1, 1], num_frames),
                           np.linspace([1, 1, 1], self.cube2_scale, num_frames),
                           np.linspace([1, 1, 1], pic_scale * 2, num_frames)])

        scales[0, :, 2] = np.linspace(1, 0, num_frames)
        scales[3, :, 1] = np.linspace(1, 0, num_frames)

        focus_points = np.linspace([0, 0, 0], self.focus_point, num_frames)

        for i in range(num_frames):
            frame = self.canvas.create_frame()

            for cube, pos, scale in zip(self.cubes, positions, scales):
                transform = sp.Transforms.translate(pos[i]) @ sp.Transforms.scale(scale[i])
                frame.add_mesh(cube, transform=transform)

            transform = sp.Transforms.translate(scene_positions[i]) @ sp.Transforms.scale(scene_scales[i])
            frame.add_mesh(self.scene_mesh, transform=transform)

            transform = sp.Transforms.translate(pic_positions[i]) @ sp.Transforms.scale(pic_scales[i])
            frame.add_mesh(self.pic_mesh, transform=transform)
            frame.camera = self._create_camera(focus_points[i])
            frame.focus_point = sp.FocusPoint(focus_points[i])

    def _create_cube(self, color: np.ndarray) -> sp.Mesh:
        cube = self.scene.create_mesh(shared_color=color, layer_id="cubes")
        cube.add_cube(transform=sp.Transforms.scale(0.5))
        return cube

    def _animate_still(self):
        # this creates the ScenePic logo
        meshes = [self.scene_mesh, self.cubes[1], self.cubes[2], self.pic_mesh]
        transforms = [
            sp.Transforms.translate(self.scene_pos),
            sp.Transforms.translate(self.cube1_pos),
            sp.Transforms.translate(self.cube2_pos) @ sp.Transforms.scale(self.cube2_scale),
            sp.Transforms.translate(self.pic_pos)
        ]

        for _ in range(self.num_still_frames):
            frame = self.canvas.create_frame()

            for mesh, transform in zip(meshes, transforms):
                frame.add_mesh(mesh, transform=transform)

            frame.camera = self._create_camera(self.focus_point)
            frame.focus_point = sp.FocusPoint(self.focus_point)


def _main():
    anim = Animation()
    anim.save("hello_scenepic.html")


if __name__ == "__main__":
    _main()
