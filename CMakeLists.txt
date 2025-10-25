cmake_minimum_required(VERSION 3.20.2)

project(rt
  VERSION       0.1.0.0
  # HOMEPAGE_URL  "https://github.com/seanwilliamcarroll/interpreters"
)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD            20 )
set(CMAKE_CXX_STANDARD_REQUIRED   ON )
set(CMAKE_CXX_EXTENSIONS          OFF)

add_compile_options(-Wall)

add_subdirectory(core)
add_subdirectory(rt)

#*****************************************************************************
