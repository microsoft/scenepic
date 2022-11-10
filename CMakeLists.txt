# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.
 
cmake_minimum_required( VERSION 3.15 )

include(FetchContent)

# -------------------- Version --------------------------------

file( STRINGS "VERSION" SCENEPIC_VERSION_FILE )

string( REPLACE "." ";" SCENEPIC_VERSION_LIST ${SCENEPIC_VERSION_FILE} )

list( GET SCENEPIC_VERSION_LIST 0 SCENEPIC_VERSION_MAJOR )

list( GET SCENEPIC_VERSION_LIST 1 SCENEPIC_VERSION_MINOR )

list( GET SCENEPIC_VERSION_LIST 2 SCENEPIC_VERSION_REVISION )

set( SCENEPIC_VERSION ${SCENEPIC_VERSION_MAJOR}.${SCENEPIC_VERSION_MINOR}.${SCENEPIC_VERSION_REVISION} )

message("Configure SCENEPIC_VERSION at ${SCENEPIC_VERSION}")

project( scenepic VERSION ${SCENEPIC_VERSION} LANGUAGES CXX)

# -------------------- Options --------------------------------

option( SCENEPIC_BUILD_DOCUMENTATION "Specifies whether to build the documentation for the API and XML" OFF )
option( SCENEPIC_BUILD_PYTHON "Specifies whether to build the python module" OFF )
option( SCENEPIC_BUILD_TESTS "Specifies whether to build the tests" OFF )
option( SCENEPIC_BUILD_EXAMPLES "Specifies whether to build the examples" OFF )
option( SCENEPIC_FORMAT "Specifies whether to enable the ability to format code via clang-format" OFF )

if( NOT DEFINED CMAKE_BUILD_TYPE )
  SET(CMAKE_BUILD_TYPE Debug)
endif()

# ------------------- Detect the system -----------------------

set (CMAKE_CXX_STANDARD 14)

if( APPLE )
  add_definitions( -DAPPLE )
  set( CMAKE_MACOSX_RPATH ON )
  set( SYSTEM_NAME "Mac" )
  set( SCENEPIC_NUGET_LIB "libscenepic.a")
  execute_process(COMMAND sysctl kern.ostype OUTPUT_VARIABLE OSX_ID)
  execute_process(COMMAND sysctl kern.osrelease OUTPUT_VARIABLE OSX_RELEASE)
  if( LSB_ID AND LSB_RELEASE )
    string(STRIP "${OSX_ID}" OSX_ID)
    string(STRIP "${OSX_RELEASE}" OSX_RELEASE)
    set( SYSTEM_NAME "${SYSTEM_NAME}-${OSX_ID}-${OSX_RELEASE}")
  endif()
elseif( UNIX )
  set( SYSTEM_NAME "Linux" )
  set( SCENEPIC_NUGET_LIB "libscenepic.a")
  execute_process(COMMAND lsb_release -is OUTPUT_VARIABLE LSB_ID)
  execute_process(COMMAND lsb_release -rs OUTPUT_VARIABLE LSB_RELEASE)
  if( LSB_ID AND LSB_RELEASE )
    string(STRIP "${LSB_ID}" LSB_ID)
    string(STRIP "${LSB_RELEASE}" LSB_RELEASE)
    set( SYSTEM_NAME "${SYSTEM_NAME}-${LSB_ID}-${LSB_RELEASE}")
  elseif( EXISTS "/etc/debian_version")
    file( READ /etc/debian_version DEBIAN_VERSION )
    set( SYSTEM_NAME "${SYSTEM_NAME}-${DEBIAN_VERSION}")
  elseif( EXISTS "/etc/os-release")
    execute_process(COMMAND "sed" "-ne" "s/^ID=\"\\?\\([a-z]\\+\\)\"\\?$/\\1/p" "/etc/os-release" OUTPUT_VARIABLE OS_RELEASE_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND "sed" "-ne" "s/^VERSION_ID=\"\\?\\([0-9\\.]\\+\\)\"\\?$/\\1/p" "/etc/os-release" OUTPUT_VARIABLE OS_RELEASE_VERSION_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
    set( SYSTEM_NAME "${SYSTEM_NAME}-${OS_RELEASE_ID}-${OS_RELEASE_VERSION_ID}")
  elseif( EXISTS "/etc/redhat-release")
    set( SYSTEM_NAME "${SYSTEM_NAME}-Redhat")
  endif()
elseif( WIN32 )
  set( SCENEPIC_NUGET_LIB "scenepic.lib")
  set( SYSTEM_NAME "Windows" )
endif()
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set( SYSTEM_NAME "${SYSTEM_NAME}-64bit" )
  set( SYSTEM_BITS "x64" )
else()
  set( SYSTEM_NAME "${SYSTEM_NAME}-32bit" )
  set( SYSTEM_BITS "x86" )
endif()

# -------------------- Find packages --------------------------

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen
  GIT_TAG        3.4.0
)

FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11/
  GIT_TAG        v2.10.1
)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) 

if(NOT TARGET Eigen3::Eigen)
  FetchContent_GetProperties(eigen)
  if(NOT eigen_POPULATED)
    FetchContent_Populate(eigen)
    set( BUILD_TESTING OFF )
    add_subdirectory(${eigen_SOURCE_DIR} ${eigen_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  if(NOT TARGET Eigen3::Eigen)
    add_library(Eigen3::Eigen ALIAS eigen)
  endif()
endif()

if( SCENEPIC_BUILD_DOCUMENTATION )
  set( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR}/ci/doxygen )
  find_package( Doxygen REQUIRED )
endif()

if( SCENEPIC_BUILD_PYTHON )
  FetchContent_GetProperties(pybind11)
  if(NOT pybind11_POPULATED)
    FetchContent_Populate(pybind11)
    set( BUILD_TESTING OFF )
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
endif()

# -------------------- Build settings -----------------------------

if( NOT DEFINED SCENEPIC_JS_LIB_PATH )
   set( SCENEPIC_JS_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/dist/scenepic.min.js CACHE PATH "Javascript library path" )
endif()

file( READ ${SCENEPIC_JS_LIB_PATH} SCENEPIC_JS_LIB_RAW )
string(LENGTH "${SCENEPIC_JS_LIB_RAW}" SCENEPIC_JS_LIB_RAW_LENGTH)
math(EXPR SCENEPIC_JS_LIB_RAW_LENGTH "${SCENEPIC_JS_LIB_RAW_LENGTH} - 1")
foreach(iter RANGE 0 ${SCENEPIC_JS_LIB_RAW_LENGTH} 2048)
  string(SUBSTRING "${SCENEPIC_JS_LIB_RAW}" ${iter} 2048 line)
  string(APPEND SCENEPIC_JS_LIB_SRC "${line})js\",\n\tR\"js(")
endforeach()

# -------------------- Testing ------------------------------------

if( SCENEPIC_BUILD_TESTS )
  set( BUILD_TESTING ON )
  include( CTest )
endif()

# -------------------- Walk the subdirectories --------------------

set( CPP_TARGETS
  scenepic
)

add_subdirectory( src/scenepic )
if( SCENEPIC_BUILD_EXAMPLES OR SCENEPIC_BUILD_TESTS )
  add_subdirectory( src/examples )
  list( APPEND CPP_TARGETS scenepic_examples )
endif()
if( SCENEPIC_BUILD_TESTS )
  add_subdirectory( test )
  list( APPEND CPP_TARGETS scenepic_tests )
endif()
if( SCENEPIC_BUILD_DOCUMENTATION )
  add_subdirectory( src/doc )
  list( APPEND CPP_TARGETS scenepic_doc )
endif()

add_custom_target( cpp )
add_dependencies( cpp ${CPP_TARGETS} )

# -------------------- Format -------------------------------------

if( SCENEPIC_FORMAT )
  find_program(CLANG_FORMAT NAMES clang-format-10 clang-format-14 REQUIRED )
  file(GLOB_RECURSE ALL_SOURCE_FILES CONFIGURE_DEPENDS
       src/examples/*.cpp
       src/scenepic/*.cpp
       src/scenepic/*.h
       src/include/scenepic/*
       test/*.cpp
       test/*.h
  )

  add_custom_target(scenepic_format
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMAND ${CLANG_FORMAT}
                    -i
                    ${ALL_SOURCE_FILES})

endif()

# -------------------- Install ------------------------------------

set(INSTALL_CONFIGDIR cmake)
set(INSTALL_LIBDIR build/native/lib)
set(INSTALL_INCLUDEDIR build/native/include)

install(TARGETS scenepic 
  EXPORT scenepic-targets
  ARCHIVE DESTINATION ${INSTALL_LIBDIR}
  LIBRARY DESTINATION ${INSTALL_LIBDIR}
)

if( WIN32 )
  if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    install(FILES
      $<TARGET_FILE_DIR:scenepic>/scenepic.pdb
      DESTINATION ${INSTALL_LIBDIR}
    )
  endif()
endif()

install(DIRECTORY src/include/ DESTINATION ${INSTALL_INCLUDEDIR})
install(FILES 
  ${CMAKE_CURRENT_BINARY_DIR}/src/scenepic/version.h
  ${CMAKE_CURRENT_BINARY_DIR}/src/scenepic/js_lib.h
  DESTINATION ${INSTALL_INCLUDEDIR}/scenepic
)

install(EXPORT scenepic-targets
  FILE
    scenepicTargets.cmake
  NAMESPACE
    scenepic::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/scenepicConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/scenepicConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/scenepicConfig.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

INSTALL(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/scenepicConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/scenepicConfigVersion.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
)

# -------------------- Export -------------------------------------

export(EXPORT scenepic-targets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/scenepicTargets.cmake
  NAMESPACE scenepic::
)

export(PACKAGE scenepic)

# -------------------- Package ------------------------------------

set( PROJECT_FILES
  README.md
  CHANGELOG.md
)

# copy these files into the root of the distribution zip
install( FILES ${PROJECT_FILES} DESTINATION "." )

# NuGet files
set( SCENEPIC_NUGET_NAME "scenepic-v${MSVC_TOOLSET_VERSION}-${SYSTEM_BITS}-${CMAKE_BUILD_TYPE}" CACHE STRING "SCENEPIC NuGet Name" FORCE )
file( READ RELEASE_NOTES SCENEPIC_RELEASE_NOTES )

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ci/nuget/template.nuspec.in" "${CMAKE_CURRENT_BINARY_DIR}/nuget/${SCENEPIC_NUGET_NAME}.nuspec" @ONLY )
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/ci/nuget/template.targets.in"
               "${CMAKE_CURRENT_BINARY_DIR}/nuget/build/native/${SCENEPIC_NUGET_NAME}.targets" @ONLY )

set( CPACK_SYSTEM_NAME ${SYSTEM_NAME} )
set( CPACK_PACKAGE_VERSION "${SCENEPIC_VERSION}" )
set( CPACK_GENERATOR "ZIP" )
set( CPACK_SOURCE_GENERATOR "ZIP" )
set( CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0 )
include( CPack )
