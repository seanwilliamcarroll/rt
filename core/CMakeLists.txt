
project(core VERSION 0.1.0)

#*****************************************************************************

add_library(core STATIC
  src/color.cpp
  src/interval.cpp
  src/aabb.cpp)

target_include_directories(core PUBLIC inc)

#*****************************************************************************

# add_executable(core-test
#   test/src/main.cpp
#   test/src/token_test.cpp
#   test/src/core_lexer_test.cpp
#   test/src/example_test.cpp)

# target_include_directories(core-test PUBLIC test/inc)

# target_link_libraries(core-test
#   core
#   rapidcheck
#   doctest::doctest)

# if (APPLE)
#   set_target_properties(core-test PROPERTIES
#     BUILD_RPATH "/opt/local/libexec/llvm-17/lib"
#   )
# endif (APPLE)
  
#*****************************************************************************

# add_test(NAME core.example COMMAND core-test -ts=core.example)
# add_test(NAME core.token COMMAND core-test -ts=core.token)
# add_test(NAME core.lexer COMMAND core-test -ts=core.lexer)

#*****************************************************************************
