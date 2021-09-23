# --------------------------------------------------------------------------------------------------------------------
# Copyright (C) Microsoft Corporation.  All rights reserved.
# --------------------------------------------------------------------------------------------------------------------

set( ASSETS
  Analog.png
  hand.obj
  Hello.png
  Science.png
  hello.mp3
  jelly.obj
  jelly.png
  red.ogg
  green.ogg
  blue.ogg
  circles.mp4
  cameras.json
  render0.png
  render1.png
  render2.png
  PolarBear.png
)

if( WIN32 )
  set( EXAMPLES_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG> )
else()
  set( EXAMPLES_FOLDER ${CMAKE_CURRENT_BINARY_DIR} )
endif()

set( EXAMPLES
  hello_world
  getting_started
  mesh_animation
  waves
  audio_tracks
  circles_video
  multiview
)

foreach( example ${EXAMPLES})
  add_executable( ${example} ${example}.cpp )
  target_link_libraries( ${example} scenepic::scenepic )
  target_compile_features(${example} PRIVATE cxx_std_14)
  add_test( NAME ${example}
            COMMAND ${example}
            WORKING_DIRECTORY ${EXAMPLES_FOLDER})
endforeach()

add_custom_target( scenepic_examples )
add_dependencies( scenepic_examples ${EXAMPLES} )

foreach( asset ${ASSETS})
  add_custom_command( TARGET scenepic_examples POST_BUILD COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${PROJECT_SOURCE_DIR}/ci/assets/${asset} ${EXAMPLES_FOLDER}/${asset} )
endforeach()
