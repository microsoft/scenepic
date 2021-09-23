// ----------------------------------------------------------------------------
//
// getting_started.cpp -- ScenePic Getting Started example app
//
// Copyright (C) 2019 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>

#include <algorithm>
#include <string>
#include <vector>

#include <Eigen/Core>

#include "svt.h"


int main(int argc, char *argv[])
{
    // when we build a ScenePic we are essentially building a web
    // page, and the ScenePic will automatically populate parts of
    // that webpage. If you do not provide a template, it will
    // set up a series of floating divs, but for this example
    // let's provide a simple structure.
    std::string body_html = R"svthtml(
<table>
<tr>
    <td id="main" rowspan=3></td>
    <td><h2>X</h2></td>
    <td id="projx"></td>
</tr>
<tr><td><h2>Y</h2></td><td id="projy"></tr>
<tr><td><h2>Z</h2><td id="projz"></td></tr>
</tr>
</table>
)svthtml";

    // the scene object acts as the root of the entire ScenePic environment
    svt::Scene scene;

    // you can use it to create one or more canvases to display 3D or 2D
    // objects. Canvas objects display Frames. For a static ScenePic, there
    // will only be a single Frame, but you can use multiple frames to create
    // an animation or to display a range of visualizations in the same visual
    // space. We will create one 3D canvas to display the full scene, and then
    // some 2D canvases which will show projections of the scene.
    auto main = scene.create_canvas_3d("main", 600, 600, "main");
    auto projx = scene.create_canvas_2d("projx", 200, 200, "projx");
    auto projy = scene.create_canvas_2d("projy", 200, 200, "projy");
    auto projz = scene.create_canvas_2d("projz", 200, 200, "projz");

    std::vector<std::shared_ptr<svt::Canvas2D>> projections = {projx, projy, projz};

    // the scene object is also used to create Mesh objects that will be added
    // to frames. We are going to create an animation of some spheres orbiting
    // a fixed cube, so let's create a default unit cube to start.
    auto cube = scene.create_mesh("cube");

    // the Mesh object has a variety of methods for adding primitive objects
    // or loading arbitrary mesh geometry.
    cube->add_cube(svt::Colors::White);

    // let's create our spheres as well, using some different colors
    std::vector<svt::Color> sphere_colors = {svt::Colors::Red, svt::Colors::Green, svt::Colors::Blue};
    std::vector<std::shared_ptr<svt::Mesh>> spheres;
    std::transform(sphere_colors.begin(), sphere_colors.end(), std::back_inserter(spheres), [&](const svt::Color &color){
        auto mesh = scene.create_mesh();
        mesh->add_sphere(color, svt::Transforms::scale(0.5));
        return mesh;
    });

    // now we will iteratively create each frame of the animation.
    for(int i=0; i<180; ++i)
    {
        // first we create a frame object. This will be used to populate
        // the 3D canvas.
        auto main_frame = main->create_frame();

        // first we add the cube to the frame
        main_frame->add_mesh(cube);

        // Next, we add the spheres. ScenePic has a variety of useful tools
        // for operating on 3D data. Some of the most useful enable us to
        // create transforms to move meshes around. Let's create the
        // transforms for our three rotating spheres and add them to the frame.
        // The C++ interface uses Eigen to handle matrices and vectors.
        Eigen::MatrixXf positions(3, 4);
        positions << 1, 0, 0, 1,
                     0, 1, 0, 1,
                     0, 0, 1, 1;
        auto inc = static_cast<float>(2.0 * M_PI / 180.0);
        positions.row(0) = svt::Transforms::rotation_about_y(inc * i) * positions.row(0).transpose();
        positions.row(1) = svt::Transforms::rotation_about_z(2 * inc * i) * positions.row(1).transpose();
        positions.row(2) = svt::Transforms::rotation_about_x(3 * inc * i) * positions.row(2).transpose();
        positions.conservativeResize(3, 3);
        for(auto j=0; j<spheres.size(); ++j)
        {
            auto transform = svt::Transforms::translate(positions.row(j));
            main_frame->add_mesh(spheres[j], transform);
        }

        // now we'll populate our projections
        for(auto j=0; j<projections.size(); ++j)
        {
            auto proj = projections[j];
            auto proj_frame = proj->create_frame();

            // 2D frames work in pixels (as oppose to world units) so we need
            // to convert positions to pixels.
            proj_frame->add_rectangle(75, 75, 50, 50, svt::Colors::Black, 1.0f, svt::Colors::White);
            for(auto k=0; k<sphere_colors.size(); ++k)
            {
                float x = positions(k, j) * 50 + 100;
                float y = positions(k, (j + 1) % positions.cols()) * 50 + 100;
                proj_frame->add_circle(x, y, 12.5, svt::Colors::Black, 1.0f, sphere_colors[k]);
            }
        }
    }

    // this will make user interactions happen to all canvas simultaneously
    scene.link_canvas_events({"main", "projx", "projy", "projz"});

    // The scene is complete, so we write it to an HTML file. We have the
    // choice to either embed the ScenePic javascript library (which we do here)
    // or to use a web-hosted one instead. This can reduce overall file size,
    // but embedding will allow the file to be viewed entirely offline.
    scene.save_as_html("getting_started.html", "Getting Started", true, "", body_html);
}