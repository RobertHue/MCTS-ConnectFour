# Author:	Robert Huemmer
cmake_minimum_required (VERSION 3.2 FATAL_ERROR)

# Define the project name
project(MCTS_ConnectFour VERSION 0.1 LANGUAGES CXX) 

message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")
message(STATUS "Target: ${CMAKE_SYSTEM_PROCESSOR} (${CMAKE_SYSTEM_NAME})")
message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
 
########################################
# set some compiler flags
SET(COMPILER_FLAGS	
	"-std=c++11 -g3 -Wall"
)
SET(LINKER_FLAGS
	"" 
)
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS}" )
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}" )

########################################
# configure and find BOOST
set(Boost_USE_STATIC_LIBS        ON) 	# only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.62.0 REQUIRED COMPONENTS system filesystem unit_test_framework serialization)

########################################
# configure and find Qt5 (uncomment when used)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)	# Find includes in corresponding build directories
#set(CMAKE_AUTOMOC ON)	# Instruct CMake to run moc automatically when needed.
#find_package(Qt5 REQUIRED COMPONENTS Widgets)

########################################
# Add a given directory to those the compiler uses to search for include files
include_directories( includes )
include_directories (${Boost_INCLUDE_DIRS})

########################################

# Add a subdirectory to the project. The CMakeLists.txt in that 
# subdirectory will be used for further setting up  the project.
# CMake creates a Makefiles/Solution for each subproject
add_subdirectory( src )

enable_testing()
add_subdirectory( tests )
