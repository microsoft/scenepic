ScenePic: 3D Visualization Made Easy
====================================

© Microsoft 2021

Primary Contact: <matjoh@microsoft.com>

All platforms have good support for 2D images, with well-recognized formats
such as PNG and JPEG that can be viewed out of the box (no installation)
and shared trivially.

However, while many formats exist for 3D data, none are well-supported
without installation of tools such as MeshLab, Blender, etc.

ScenePic was created for 3D computer vision researchers such as those
working on [HoloLens](https://www.microsoft.com/en-gb/hololens)
and [Mesh](https://www.microsoft.com/en-us/mesh) at Microsoft.
It was designed to be a lightweight, reuseable 3D visualization
library, with the following desiderata in mind:

- Make experimentation with 3D data near effortless
- Incredibly easy to create and share 3D results

  - zero-install sharing of detailed 3D results using HTML
  - based on modern web standards so usable with any modern browser
  - embeddable in other HTML documents

- Performant

  - based on WebGL

- High quality visuals
- Works both offline or interactively in client-server setup
- Simple, clean API

  - friendly Python front-end
  - basic mesh json file format
  - other language front ends easy to add


.. image:: ../example_render.png


Getting Started
---------------

.. literalinclude:: ../../python_examples/getting_started.py
   :language: python
   :linenos:

The resulting ScenePic looks like this:

.. image:: ../getting_started.gif

Using the HTML Client
---------------------

.. image:: ../scenepic_ui.png

A ScenePic HTML page will look something like the image above. This example shows
four *Canvas* objects, each of which contains several *Frame* objects (refering under the
hood to a set of *Mesh* objects).

The UI supports standard 3D mouse controls (drag to rotate, shift-drag to
translate in xy, mousewheel to translate in z) to move the viewport camera.
On touch screens: single finger to rotate, two fingers to translate in xyz.
You can slow any of the mouse controls by holding down the Alt key. If you
accidentally transform the camera too wildly, you can reset by pressing 'r'.

In the top right of each canvas a *Layer* control will appear.
This allows the user to toggle certain layers of meshes on and off
interactively.

Each *Frame* for a 3D canvas has an associated *Focus Point* - a 3D
location about which the camera rotates.  You can view the *Focus Point* by
holding down the '``' (backtick) key, and while holding this down it can be
translated using the mouse.  If you press 'l' then the camera is 'locked' to the focus
point in xy, though you can still rotate and translate in z.  Locking to
the focus point is particularly useful to center useful content in
animations where the focus point can be different in each frame. You
can also toggle the camera automatically orbiting the focus point by
pressing '\\'.

For *Scene* objects that contain many frames, you can control animation by
pressing the spacebar to play/pause or by using the playback control
widget.  You can also use the scrollbar to select different frames.


.. toctree::
    :maxdepth: 3

    api