cmake_minimum_required (VERSION 3.20 FATAL_ERROR)
set (CMAKE_CXX_STANDARD 20)

#-----------------------------------------------------------------------------
# Define the project name
########################################
set(PROJECT_NAME "MCTS_ConnectFour")
set(EXECUTABLE_NAME "MCTS_ConnectFour")
set(TEST_NAME "MCTS_ConnectFour_tests")
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)

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

#-----------------------------------------------------------------------------
# include directories for the compiler to search for (all targets)
########################################
include_directories("include")


#-----------------------------------------------------------------------------
# configure and find BOOST
########################################
set(Boost_USE_STATIC_LIBS        ON) 	# only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.69.0 REQUIRED COMPONENTS system filesystem unit_test_framework serialization)



#-----------------------------------------------------------------------------
# configure and find QT5
set(CMAKE_INCLUDE_CURRENT_DIR ON)	# Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON)	# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOUIC OFF)	# Create code from a list of Qt designer ui files
find_package(Qt5 REQUIRED)
find_package(Qt5Widgets REQUIRED)


# looking for all dependencies
file(GLOB_RECURSE QT_DEPS "qt/*.hpp" "qt/*.h" "qt/*.cpp" "qt/*.cc")
file(GLOB_RECURSE SOURCES_DEPS "src/*.cpp" "src/*.cc")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR : ${CMAKE_CURRENT_SOURCE_DIR}")

list(REMOVE_ITEM SOURCES_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp) # remove main.cpp so its not included inside test build
file(GLOB_RECURSE HEADERS_DEPS "include/*.hpp" "include/*.h")
set(DEPENDENCIES ${SOURCES_DEPS} ${HEADERS_DEPS})
message(STATUS "DEPENDENCIES : ${DEPENDENCIES}")

#add source directory
add_subdirectory("src")
add_subdirectory("tests")


##### cmake code from the past which should be not used (bad practice)
# add_definitions("-Wall")
# usage:  enables all gcc compiler warning messages
# disadvantage: applies it to all directories and sources below
# alternative: target_compile_options(target PRIVATE -Wall) applies the property -Wall on the target Foo in PRIVATE-scope

# include_directories(${PROJECT_SOURCE_DIR}/src/)
# usage: adds directories specified to the search path, which are used for some compilers when including files.
# disadvantage: command is leaked to all targets in the same scope (directories and subsources)
# alternative: target_include_directories( target
#    PUBLIC
#        public
#    PRIVATE
#        src)

# link_libraries
# alternative: target_link_libraries()

# set(CMAKE_CXX_FLAGS -std=c++11)
# usage: set the c++ standard to compile the all targets
# disadvantage: it applies the standard compile for every target
# alternative: target_compile_features(target PRIVATE/INTERFACE/PUBLIC cxx_std_11)

# file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
# usage:
