"""Example script demonstrating the basic ScenePic functionality."""

import numpy as np
import svt


def _main():
    # when we build an ScenePic we are essentially building a web
    # page, and the ScenePic will automatically populate parts of
    # that webpage. If you do not provide a template, it will
    # set up a series of floating divs, but for this example
    # let's provide a simple structure.
    body_html = """
<table>
<tr>
    <td id="main" rowspan=3></td>
    <td><h2>X</h2></td>
    <td id="projx"></td>
</tr>
<tr><td><h2>Y</h2></td><td id="projy"></tr>
<tr><td><h2>Z</h2><td id="projz"></td></tr>
</table>
"""

    # the scene object acts as the root of the entire ScenePic environment
    scene = svt.Scene()

    # you can use it to create one or more canvases to display 3D or 2D
    # objects. Canvas objects display Frames. For a static ScenePic, there
    # will only be a single Frame, but you can use multiple frames to create
    # an animation or to display a range of visualizations in the same visual
    # space. We will create one 3D canvas to display the full scene, and then
    # some 2D canvases which will show projections of the scene.
    main = scene.create_canvas_3d(width=600, height=600, html_id="main")
    projx = scene.create_canvas_2d(width=200, height=200, html_id="projx")
    projy = scene.create_canvas_2d(width=200, height=200, html_id="projy")
    projz = scene.create_canvas_2d(width=200, height=200, html_id="projz")

    # the scene object is also used to create Mesh objects that will be added
    # to frames. We are going to create an animation of some spheres orbiting
    # a fixed cube, so let's create a default unit cube to start.
    cube = scene.create_mesh("cube")

    # the Mesh object has a variety of methods for adding primitive objects
    # or loading arbitrary mesh geometry. In this example, we will just
    # be using primitives, but th python tutorial shows all the various
    # methods for adding geometry to a mesh.
    cube.add_cube(color=svt.Colors.White)

    # let's create our spheres as well, using some different colors
    sphere_colors = [svt.Colors.Red, svt.Colors.Green, svt.Colors.Blue]
    spheres = []
    for i, color in enumerate(sphere_colors):
        sphere = scene.create_mesh("sphere_{}".format(i))
        sphere.add_sphere(color=color, transform=svt.Transforms.scale(0.5))
        spheres.append(sphere)

    # now we will iteratively create each frame of the animation.
    for i in range(180):
        # first we create a frame object. This will be used to populate
        # the 3D canvas.
        main_frame = main.create_frame()

        # first we add the cube to the frame
        main_frame.add_mesh(cube)

        # Next, we add the spheres. ScenePic has a variety of useful tools
        # for operating on 3D data. Some of the most useful enable us to
        # create transforms to move meshes around. Let's create the
        # transforms for our three rotating spheres and add them to the frame.
        # The Python interface uses numpy for matrices and vectors.
        positions = np.concatenate([np.eye(3, dtype=np.float32) * 1.3,
                                    np.ones((3, 1), dtype=np.float32)], axis=-1)
        inc = 2 * np.pi / 180
        positions[0] = svt.Transforms.RotationAboutYAxis(inc * i) @ positions[0].T
        positions[1] = svt.Transforms.RotationAboutZAxis(2 * inc * i) @ positions[1].T
        positions[2] = svt.Transforms.RotationAboutXAxis(3 * inc * i) @ positions[2].T
        positions = positions[:, :3]
        for sphere, position in zip(spheres, positions):
            transform = svt.Transforms.translate(position)
            main_frame.add_mesh(sphere, transform=transform)

        # now we'll populate our projections
        for j, proj in enumerate([projx, projy, projz]):
            proj_frame = proj.create_frame()

            # 2D frames work in pixels (as oppose to world units) so we need
            # to convert positions to pixels.
            proj_frame.add_rectangle(75, 75, 50, 50, fill_color=svt.Colors.White)
            points = np.roll(positions, j, axis=1)[:, 1:]
            points[:, 1] *= -1
            points = points * 50 + 100

            for point, color in zip(points, sphere_colors):
                proj_frame.add_circle(point[0], point[1], 12.5, fill_color=color)

    # this will make user interactions happen to all canvas simultaneously
    scene.link_canvas_events(main, projx, projy, projz)

    # The scene is complete, so we write it to an HTML file. We have the
    # choice to either embed the ScenePic javascript library (which we do here)
    # or to use a web-hosted one instead. This can reduce overall file size,
    # but embedding will allow the file to be viewed entirely offline.
    scene.save_as_html("getting_started.html", embed_library=True, title="Getting Started", body_html=body_html)


if __name__ == "__main__":
    _main()
