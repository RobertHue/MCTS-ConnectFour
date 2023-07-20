cmake_minimum_required (VERSION 3.20 FATAL_ERROR)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#-----------------------------------------------------------------------------
# Define the project name
########################################
set(PROJECT_NAME "MCTS_ConnectFour")
set(EXECUTABLE_NAME "MCTS_ConnectFour")
set(TEST_NAME "MCTS_ConnectFour_tests")
project(${PROJECT_NAME} VERSION 1.0 LANGUAGES CXX)

#-----------------------------------------------------------------------------
# generate a message
########################################
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")
message(STATUS "Target: ${CMAKE_SYSTEM_PROCESSOR} (${CMAKE_SYSTEM_NAME})")
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#-----------------------------------------------------------------------------
# set cmake module path
########################################
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory("src")
add_subdirectory("tests")
