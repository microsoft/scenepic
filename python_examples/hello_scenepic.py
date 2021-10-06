import os

import scenepic as sp


def _asset(name: str):
    asset_dir = os.path.join(os.path.dirname(__file__), "..", "ci", "assets")
    return os.path.join(asset_dir, name)


def _main():
    scene_obj = sp.load_obj(_asset("scene.obj"))
    pic_obj = sp.load_obj(_asset("pic.obj"))

    scene = sp.Scene()
    canvas = scene.add_canvas_3d(width=1280, height=640)

    scene_mesh = scene.create_mesh()
    
    scene.
    
    
