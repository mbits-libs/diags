# Copyright (c) 2020 midnightBITS
# This file is licensed under MIT license (see LICENSE for details)

cmake_minimum_required (VERSION 3.12)
project (diags
  VERSION 0.9.6
  DESCRIPTION "Diagnostic library with source positions, ranges and hints."
  LANGUAGES CXX)

set(PROJECT_VERSION_STABILITY "") # "", or "-alpha", or "-beta", or "-rc.5"

set(DIAGS_BUILD_FOR_CONAN OFF CACHE BOOL "Changes the conan requirement handling")
set(DIAGS_BUILD_AS_STANDALONE ${DIAGS_BUILD_FOR_CONAN} CACHE BOOL "Force build as if standalone")

if (DIAGS_BUILD_AS_STANDALONE OR CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  message(STATUS "diag: Standalone")
  set(DIAGS_STANDALONE ON)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Standalone will be compiled with C++20 to suppport char8_t,
  # clients need at least 17 to compile (for string_view for instance).
  set(STANDARD 20 CACHE BOOL "Tweak the required standard")
  set(CMAKE_CXX_STANDARD ${STANDARD})
  set(CMAKE_CXX_STANDARD_REQUIRED OFF)
  set(CMAKE_CXX_EXTENSIONS OFF)

  find_package(Python3 COMPONENTS Interpreter REQUIRED)

  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_BINARY_DIR}/conan")
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${PROJECT_BINARY_DIR}/conan")

  find_program(CONAN_COMMAND NAMES conan conan.bat)
else()
  message(STATUS "diag: Subdir")
  set(DIAGS_STANDALONE OFF)
endif()

set(DIAGS_TESTING ${DIAGS_STANDALONE} CACHE BOOL "Compile and/or run self-tests")
set(DIAGS_INSTALL ${DIAGS_STANDALONE} CACHE BOOL "Install the library")

if (DIAGS_STANDALONE)
  set(CONAN_CMAKE_SILENT_OUTPUT ON)
  if (DIAGS_TESTING)
    find_package(GTest REQUIRED CONFIG)
  endif()
  find_package(fmt REQUIRED CONFIG)
  find_package(mbits-semver REQUIRED CONFIG)
endif()

if (DIAGS_TESTING)
  set(COVERALLS_PREFIX DIAGS_)
  list(APPEND DIAGS_COVERALLS_DIRS
    include/diags
    src
  )

  include(tools/coveralls/Coveralls.cmake)
endif()

# See <https://github.com/lefticus/cppbestpractices/blob/v1.0.0/02-Use_the_Tools_Available.md#compilers>
if (MSVC)
  set(DIAGS_ADDITIONAL_WALL_FLAGS
      /permissive-
      /Zc:__cplusplus
      /W4
      /w14242
      /w14254
      /w14263
      /w14265
      /w14287
      /we4289
      /w14296
      /w14311
      /w14545
      /w14546
      /w14547
      /w14549
      /w14555
      /w14619
      /w14640
      /w14826
      /w14905
      /w14906
      /w14928
      /w14946)
else()
  set(DIAGS_ADDITIONAL_WALL_FLAGS
      -Wall -Wextra
      -Wnon-virtual-dtor
      -Wold-style-cast
      -Wcast-align
      -Wunused
      -Woverloaded-virtual
      -Wpedantic
      -Wconversion
      -Wsign-conversion
      -Wnull-dereference
      -Wdouble-promotion
      -Wformat=2
  )
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    list(APPEND DIAGS_ADDITIONAL_WALL_FLAGS -fcolor-diagnostics) # -Wlifetime
  else()
    list(APPEND DIAGS_ADDITIONAL_WALL_FLAGS
      -fdiagnostics-color
      -Wmisleading-indentation
      -Wduplicated-cond
      -Wduplicated-branches
      -Wlogical-op
      -Wuseless-cast
      )
  endif()
endif()

configure_file(src/version.hpp.in include/diags/version.hpp @ONLY)

set(SRCS
  src/printer.cpp
  src/source_code.cpp
  src/sources.cpp
  src/streams.cpp
  src/streams_file.cpp
  src/string.cpp
  src/translator.cpp
  src/version.cpp
  include/diags/diagnostic.hpp
  include/diags/printer.hpp
  include/diags/location.hpp
  include/diags/source_code.hpp
  include/diags/sources.hpp
  include/diags/streams.hpp
  include/diags/string.hpp
  include/diags/translator.hpp
  "${CMAKE_CURRENT_BINARY_DIR}/include/diags/version.hpp"
)


add_library(${PROJECT_NAME} STATIC ${SRCS})

target_compile_options(${PROJECT_NAME} PRIVATE ${DIAGS_ADDITIONAL_WALL_FLAGS})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src
)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt mbits::semver)
set_target_properties(${PROJECT_NAME} PROPERTIES
  VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

##################################################################
##  INSTALL
##################################################################

if (DIAGS_INSTALL)
  install(TARGETS ${PROJECT_NAME})
  install(DIRECTORY include/diags DESTINATION include)
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/include/diags/version.hpp" DESTINATION include/diags)
endif()

##################################################################
##  TESTING
##################################################################

if (DIAGS_TESTING)
enable_testing()
add_subdirectory(tests)
endif()

include (CPack)
