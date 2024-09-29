project(rt VERSION 0.1.0)

add_executable(rt-exe src/main.cpp)

set_target_properties(rt-exe PROPERTIES OUTPUT_NAME rt)

add_library(rt-lib STATIC
  src/rt.cpp)

set_target_properties(rt-lib PROPERTIES OUTPUT_NAME rt)

target_include_directories(rt-lib PUBLIC inc)

target_link_libraries(rt-lib core)

target_link_libraries(rt-exe rt-lib)

if (APPLE)
  set_target_properties(rt-exe PROPERTIES
    BUILD_RPATH "/opt/local/libexec/llvm-17/lib"
  )
endif(APPLE)

#****************************************************************************
