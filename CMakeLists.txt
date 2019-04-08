########################################
# Author:	Robert Huemmer
########################################
cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

########################################
# Define the project name
########################################
set(PROJECT_NAME "MCTS_ConnectFour")
set(EXECUTABLE_NAME "MCTS_ConnectFour")
set(TEST_NAME "MCTS_ConnectFour_tests")
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

########################################
# generate a message
########################################
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")
message(STATUS "Target: ${CMAKE_SYSTEM_PROCESSOR} (${CMAKE_SYSTEM_NAME})")
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

########################################
# set cmake module path
########################################
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

########################################
# include directories for the compiler to search for (all targets)
########################################
include_directories("include")

 
########################################
# configure and find BOOST
########################################
set(Boost_USE_STATIC_LIBS        ON) 	# only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.69.0 REQUIRED COMPONENTS system filesystem unit_test_framework serialization)



########################################
# configure and find QT5
########################################
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
#find_package(Qt5 REQUIRED COMPONENTS Widgets)


#set C++ standart
set (CMAKE_CXX_STANDARD 14)

# looking for all dependencies
file(GLOB_RECURSE SOURCES_DEPS "src/*.cpp" "src/*.cc")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR : ${CMAKE_CURRENT_SOURCE_DIR}") 

list(REMOVE_ITEM SOURCES_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp) # remove main.cpp so its not included inside test build
file(GLOB_RECURSE HEADERS_DEPS "include/*.hpp" "include/*.h")
set(DEPENDENCIES ${SOURCES_DEPS} ${HEADERS_DEPS}) 
message(STATUS "DEPENDENCIES : ${DEPENDENCIES}") 

#add source directory
add_subdirectory("src")
add_subdirectory("tests")
