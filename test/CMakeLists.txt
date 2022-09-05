# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
 
set( CPP_TEST_SOURCES
  scenepic_tests.cpp
  scenepic_tests.h
)

set( TESTS
  audio
  base64
  camera
  canvas2d
  canvas3d
  drop_down_menu
  frame2d
  frame3d
  graph
  image
  io
  label
  layer_settings
  matrix
  mesh_update
  primitives
  quantization
  scene
  shading
  stencil
  text_panel
  transforms
  ui_parameters
  video
)

foreach( test ${TESTS} )
  list( APPEND CPP_TEST_SOURCES "${test}.cpp" )
endforeach()

set( TEST_DRIVER scenepic_tests )
add_executable(${TEST_DRIVER} ${CPP_TEST_SOURCES})

if (TARGET scenepic_examples )
  add_dependencies(${TEST_DRIVER} scenepic_examples)
endif()

target_link_libraries( ${TEST_DRIVER} scenepic::scenepic )
target_compile_features(${TEST_DRIVER} PRIVATE cxx_std_14)
target_include_directories(${TEST_DRIVER} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src/scenepic/)

foreach( test ${TESTS} )
  add_test( NAME ${test}
            COMMAND ${TEST_DRIVER} ${test}
            WORKING_DIRECTORY $<TARGET_FILE_DIR:${TEST_DRIVER}> )
endforeach()

add_custom_command(TARGET ${TEST_DRIVER} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${TEST_DRIVER}>/assets)

add_custom_command(TARGET ${TEST_DRIVER} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/ci/assets $<TARGET_FILE_DIR:${TEST_DRIVER}>/assets)
