cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(antity_test
  LANGUAGES CXX
)

option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

include(../cmake/CPM.cmake)

CPMAddPackage("gh:google/googletest#f5e592d8ee5ffb1d9af5be7f715ce3576b8bf9c4")
CPMAddPackage("gh:TheLartians/Format.cmake@1.7.0")

if (TEST_INSTALLED_VERSION)
  find_package(antity REQUIRED)
else()
  CPMAddPackage(
    NAME antity
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..
  )
endif()

file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(antity_test ${sources})

target_link_libraries(antity_test GTest::gtest antity)

set_target_properties(antity_test PROPERTIES CXX_STANDARD 20)

enable_testing()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(GoogleTest)
gtest_discover_tests(antity_test)

if (ENABLE_TEST_COVERAGE)
  target_compile_options(antity INTERFACE -O0 -g -fprofile-arcs -ftest-coverage)
  target_link_options(antity INTERFACE -fprofile-arcs -ftest-coverage)
endif()