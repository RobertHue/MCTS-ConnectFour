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


#############################
## build doxygen
#########

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.out)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs/
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
	message(STATUS "CMAKE_CURRENT_BINARY_DIR : ${CMAKE_CURRENT_BINARY_DIR}") 
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
