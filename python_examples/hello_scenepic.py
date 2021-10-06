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
    def __init__(self,
                 scene: sp.Scene,
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
        self.num_cloud_frames = num_cloud_frames
        self.num_morph_frames = num_morph_frames
        self.num_still_frames = num_still_frames
        self.num_frames = num_cloud_frames + num_morph_frames + num_still_frames
        self.aspect_ratio = width / height

        self.colors = [sp.ColorFromBytes(242, 80, 34),
                       sp.ColorFromBytes(127, 186, 0),
                       sp.ColorFromBytes(0, 164, 239),
                       sp.ColorFromBytes(255, 185, 0)]

        self.scene = scene
        self.canvas = scene.create_canvas_3d(width=width, height=height)

        scene_obj = sp.load_obj(_asset("scene.obj"))
        self.scene_mesh = scene.create_mesh(shared_color=self.colors[0], layer_id="text")
        self.scene_mesh.add_mesh(scene_obj)
        self.scene_mesh.apply_transform(sp.Transforms.rotation_about_x(np.pi / 2))
        self.scene_pos = [0, 0, 0]

        pic_obj = sp.load_obj(_asset("pic.obj"))
        self.pic_mesh = scene.create_mesh(shared_color=self.colors[3], layer_id="text")
        self.pic_mesh.add_mesh(pic_obj)
        self.pic_pos = [1.25, -0.05, 0.5]

        self.cubes = [self._create_cube(scene, color) for color in self.colors]
        self.cube1_pos = [2, 0.15, 0.25]
        self.cube2_pos = [0.4, -0.05, 0.3]
        self.cube2_scale = [1.5, 0.2, 0.8]
        self.focus_point = [1, 0.25, 0.5]

        self.angles, self.distances = self._compute_camera_info(start_distance,
                                                                end_distance,
                                                                start_angles,
                                                                end_angles)

        self.index = 0
        self.animate_cloud(num_points)
        self.animate_morph()
        self.animate_still()

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
        angles = self.angles[self.index]
        rotation = sp.Transforms.euler_angles_to_matrix(angles)
        pos = np.array([0, 0, self.distances[self.index]], np.float32)
        pos = rotation[:3, :3] @ pos
        pos += focus_point
        self.index += 1
        return sp.Camera(pos, look_at=focus_point, aspect_ratio=self.aspect_ratio)

    def animate_cloud(self, num_points: int):
        num_frames = self.num_cloud_frames
        start_positions = np.random.uniform(-1, 1, (num_points, 3)).astype(np.float32)
        end_positions = np.zeros_like(start_positions)
        groups = [num_points // 4, num_points // 2, 3 * num_points // 4]
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

    def animate_morph(self):
        num_frames = self.num_morph_frames

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

    def _create_cube(self, scene: sp.Scene, color: np.ndarray) -> sp.Mesh:
        cube = scene.create_mesh(shared_color=color, layer_id="cubes")
        cube.add_cube(transform=sp.Transforms.scale(0.5))
        return cube

    def animate_still(self):
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


def _main():
    scene = sp.Scene()
    Animation(scene)
    scene.save_as_html("hello_scenepic.html", "Hello Scenepic!")


if __name__ == "__main__":
    _main()
