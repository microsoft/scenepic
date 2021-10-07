# --------------------------------------------------------------------------------------------------------------------
# Copyright (C) Microsoft Corporation.  All rights reserved.
# --------------------------------------------------------------------------------------------------------------------

set( ASSETS
  hand.obj
  hello.mp3
  jelly.obj
  jelly.png
  red.ogg
  green.ogg
  blue.ogg
  circles.mp4
  cameras.json
  rand15x15.png
  rand30x20.png
  render0.png
  render1.png
  render2.png
  PolarBear.png
  pic.obj
  scene.obj
  uv.png
)

if( WIN32 )
  set( EXAMPLES_FOLDER ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG> )
else()
  set( EXAMPLES_FOLDER ${CMAKE_CURRENT_BINARY_DIR} )
endif()

set( EXAMPLES
  getting_started
  hello_scenepic
  tutorial
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
