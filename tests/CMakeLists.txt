#-----------------------------------------------------------------------------
# Configure & Find BOOST
#########################
set(Boost_USE_STATIC_LIBS        ON) 	# only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS unit_test_framework)
if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
    # Other configurations or target definitions
else()
    message(FATAL_ERROR "Boost's unit_test_framework component not found.")
endif()


set(DEPENDENCIES
    # AI-headers
    ${CMAKE_SOURCE_DIR}/include/ai/GameAI.h
    ${CMAKE_SOURCE_DIR}/include/ai/Tree.h  # templated

    # Data-headers
    ${CMAKE_SOURCE_DIR}/include/data/GameState.h

    # AI-sources
    ${CMAKE_SOURCE_DIR}/src/ai/GameAI.cpp

    # Data-sources
    ${CMAKE_SOURCE_DIR}/src/data/GameState.cpp

    # MAIN
    main_tests.cpp

    # TESTS
    TestGameAI.cpp
    TestGameState.cpp
)
add_executable(
    ${TEST_NAME}
    ${DEPENDENCIES}
)
target_compile_options(
    ${TEST_NAME}
    PRIVATE -Wall -Wextra
)
target_compile_features(
    ${TEST_NAME}
    PRIVATE cxx_std_17
)
target_include_directories(
    ${TEST_NAME}
    PRIVATE ${Boost_INCLUDE_DIRS}
    PRIVATE ${CMAKE_SOURCE_DIR}/include/
)
target_link_libraries(
    ${TEST_NAME}
    PRIVATE ${Boost_LIBRARIES}
    PRIVATE Boost::unit_test_framework
)
