# Changelog

## [2022-07-06 - Version 1.0.10](https://github.com/microsoft/scenepic/releases/tag/v1.0.10)
Point release to fix issues introduced by v1.0.9.

## 2022-06-30 - Version 1.0.9
> **Warning**
> This release is not reliable on linux and should not be used.

Point release to address breaking issue with numpy 1.23.0 (#34)

## [2022-06-20 - Version 1.0.8](https://github.com/microsoft/scenepic/releases/tag/v1.0.8)
Point release updating NPM and dependency versions to address security concerns.

## [2022-02-10 - Version 1.0.7](https://github.com/microsoft/scenepic/releases/tag/v1.0.7)
Point release improving `MeshUpdate` thus adding the ability to update colors
per frame along with some minor additions.

New Features:
- Can now updated colors per frame for both meshes and instanced meshes
- `quaternion_multiply` adds the long-missing code to combine quaternion rotations
- `Camera.orbit` creates a sequence of orbiting cameras around a central point

Deprecation:
- `update_mesh_without_normals` has been deprecated in favor of the new method
  `update_mesh_positions`

## [2021-12-11 - Version 1.0.6](https://github.com/microsoft/scenepic/releases/tag/v1.0.6)
Point release adding new camera mode as well as some improvements.

Improvements:
- There is now a new camera control mode, "First Person", accessible by pressing
  <kbd>Tab</kbd>, which enables mouse-look and `wasdqe` movement controls
  (closes #22). The old mode, now called "Focus", is still available as the default.
- Font scale is now configurable on `VideoWriter`
- There is now a "reset camera" button on the control bar so that users without
  a keyboard can reset camera position


## [2021-11-16 - Version 1.0.5](https://github.com/microsoft/scenepic/releases/tag/v1.0.5)
Point release adding per-frame layer settings.

## [2021-11-2 - Version 1.0.4](https://github.com/microsoft/scenepic/releases/tag/v1.0.4)
Point release adding text burning into video frames (#5)

## [2021-10-26 - Version 1.0.3](https://github.com/microsoft/scenepic/releases/tag/v1.0.3)
Bug fix release:
- Coordinate axes bug (#4)
- Negative angle bug
- Mesh Update for instanced meshes (#8)

## [2021-10-20 - Version 1.0.2](https://github.com/microsoft/scenepic/releases/tag/v1.0.2)
Point release adding additional wheels to support Apple Silicon and some
minor UI changes.

Improvements:
- The dropdown menu visibility can be directly controlled via `UIParameters`
- A `standalone` parameter has been added to `save_as_script` to enable the
  creation of standalone scripts.
- The `status_bar_visibility` of a `Scene` can now be set (i.e. it is not
  possible to hide the status bar if desired.)

Bug Fixes:
- While transparent backgrounds were supported by the library, it was
  not exposed via the client APIs. This has been fixed via a 4D color parameter
  for `Shading`.

## [2021-10-7 - Version 1.0.1](https://github.com/microsoft/scenepic/releases/tag/v1.0.1)
Point release addressing various items that did not make the release. There will
not be a binary release as part of this change.

Improvements:
- Added new `hello_scenepic.py` and `hello_scenepic.cpp` example code
- Renamed `cppsrc` to `src`
- Added a clang-format pipeline and build targets
- Normalized C++ file format

Bug Fixes:
- `load_obj` now correctly loads normals when present in the OBJ file
- `MeshInfo` now exposes normals
- C++ build typos now fixed
- Fixed erroneous pipeline triggers


## [2021-10-4 - Version 1.0.0](https://github.com/microsoft/scenepic/releases/tag/v1.0.0)
Initial Release