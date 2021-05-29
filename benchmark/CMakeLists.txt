cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(antity_benchmark
  LANGUAGES CXX
)

include(../cmake/CPM.cmake)

CPMAddPackage("gh:TheLartians/Format.cmake@1.7.0")
CPMAddPackage("gh:martinus/nanobench@4.3.1")

CPMAddPackage(
  NAME EnTT
  VERSION 3.7.1
  GITHUB_REPOSITORY skypjack/entt
  # EnTT's CMakeLists screws with configuration options
  DOWNLOAD_ONLY True
)

if(EnTT_ADDED)
  add_library(EnTT INTERFACE)
  target_include_directories(EnTT SYSTEM INTERFACE ${EnTT_SOURCE_DIR}/src)
endif()

if (TEST_INSTALLED_VERSION)
  find_package(antity REQUIRED)
else()
  CPMAddPackage(
    NAME antity
    SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..
  )
endif()

file(GLOB_RECURSE sources CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(antity_benchmark ${sources})
target_link_libraries(antity_benchmark nanobench antity EnTT)
set_target_properties(antity_benchmark PROPERTIES CXX_STANDARD 20)