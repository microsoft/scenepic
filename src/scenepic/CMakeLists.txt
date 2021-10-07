# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
 
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/version.h" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.py.in" "${CMAKE_CURRENT_SOURCE_DIR}/version.py" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/js_lib.h.in" "${CMAKE_CURRENT_BINARY_DIR}/js_lib.h" @ONLY)

set( SOURCES
  audio_track.cpp
  cpp-base64/base64.cpp
  camera.cpp
  canvas2d.cpp
  canvas3d.cpp
  color.cpp
  constants.cpp
  drop_down_menu.cpp
  focus_point.cpp
  frame2d.cpp
  frame3d.cpp
  graph.cpp
  image.cpp
  io.cpp
  json/jsoncpp.cpp
  json_value.cpp
  label.cpp
  layer_settings.cpp
  loop_subdivision_stencil.cpp
  mesh.cpp
  mesh_info.cpp
  mesh_primitives.cpp
  mesh_update.cpp
  miniz/miniz.cpp
  scene.cpp
  scene_compression.cpp
  shading.cpp
  text_panel.cpp
  transforms.cpp
  ui_parameters.cpp
  video.cpp
  zip.cpp
)

add_library( scenepic STATIC ${SOURCES} )
add_library( scenepic::scenepic ALIAS scenepic )

target_compile_features(scenepic PRIVATE cxx_std_14)

target_include_directories( scenepic
  PUBLIC
    $<INSTALL_INTERFACE:build/native/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/scenepic>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries( scenepic
  PUBLIC
    Eigen3::Eigen
)

if( SCENEPIC_BUILD_PYTHON )
  set(PYBIND11_CPP_STANDARD -std=c++14)
  pybind11_add_module(_scenepic python.cpp ${SOURCES})

  target_include_directories( _scenepic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/scenepic
    ${CMAKE_CURRENT_BINARY_DIR}
  )
  target_link_libraries(_scenepic 
    PRIVATE
      Eigen3::Eigen
  )
  set_target_properties(_scenepic PROPERTIES
                        EXCLUDE_FROM_DEFAULT_BUILD 1
                        EXCLUDE_FROM_ALL 1)
endif()