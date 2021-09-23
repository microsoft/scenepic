# --------------------------------------------------------------------------------------------------------------------
# Copyright (C) Microsoft Corporation.  All rights reserved.
# --------------------------------------------------------------------------------------------------------------------

set( CPP_TEST_SOURCES
  svt_tests.cpp
  svt_tests.h
)

set( TESTS
  audio
  base64
  camera
  canvas2d
  canvas3d
  compression
  drop_down_menu
  frame2d
  frame3d
  graph
  image
  io
  label
  layer_settings
  mesh_update
  primitives
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

set( TEST_DRIVER svt_tests )
add_executable(${TEST_DRIVER} ${CPP_TEST_SOURCES})

if (TARGET svt_examples )
  add_dependencies(${TEST_DRIVER} svt_examples)
endif()

target_link_libraries( ${TEST_DRIVER} svt::svt )
target_compile_features(${TEST_DRIVER} PRIVATE cxx_std_14)
target_include_directories(${TEST_DRIVER} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../cppsrc/svt/)

foreach( test ${TESTS} )
  add_test( NAME ${test}
            COMMAND ${TEST_DRIVER} ${test}
            WORKING_DIRECTORY $<TARGET_FILE_DIR:${TEST_DRIVER}> )
endforeach()

add_custom_command(TARGET ${TEST_DRIVER} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${TEST_DRIVER}>/assets)

add_custom_command(TARGET ${TEST_DRIVER} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/ci/assets $<TARGET_FILE_DIR:${TEST_DRIVER}>/assets)
