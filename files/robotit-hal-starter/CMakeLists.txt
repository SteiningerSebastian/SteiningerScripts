# Top level project file for the RobotIT HAL starter kit.

cmake_minimum_required(VERSION 3.16)

# The hal/ folder is the component library.
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}")

# HAL backend: "sim" runs with no robot attached, "real" drives the board.
set(HAL_BACKEND "sim" CACHE STRING "HAL backend: real or sim")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(robotit_hal_starter)
