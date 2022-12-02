cmake_minimum_required(VERSION 2.8)

project(ergasia1)

find_package(CGAL COMPONENTS Qt5 Core)
find_package(Qt5 QUIET COMPONENTS Xml Script OpenGL Svg)

include(${CGAL_USE_FILE})

add_subdirectory(src)