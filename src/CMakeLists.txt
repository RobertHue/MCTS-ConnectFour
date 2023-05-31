#-----------------------------------------------------------------------------
# Configure & Find QT5
set(CMAKE_INCLUDE_CURRENT_DIR ON)	# Find includes in corresponding build directories
set(CMAKE_AUTOMOC ON)	# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOUIC OFF)	# Create code from a list of Qt designer ui files
find_package(Qt5 REQUIRED)
find_package(Qt5Widgets REQUIRED)
if(Qt5Widgets_FOUND)
    message(STATUS "Qt5Widgets found: ${Qt5Widgets_INCLUDE_DIRS}")
    message(STATUS "Qt5Widgets libraries: ${Qt5Widgets_LIBRARIES}")
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

    # UI-headers
    ${CMAKE_SOURCE_DIR}/include/ui/QGameAI_Thread.h
    ${CMAKE_SOURCE_DIR}/include/ui/QGameStateModel.h
    ${CMAKE_SOURCE_DIR}/include/ui/QWinnerDialog.h  # templated

    # AI-sources
    ${CMAKE_SOURCE_DIR}/src/ai/GameAI.cpp

    # Data-sources
    ${CMAKE_SOURCE_DIR}/src/data/GameState.cpp

    # UI-sources
    ${CMAKE_SOURCE_DIR}/src/ui/QGameAI_Thread.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/QGameStateModel.cpp

    # MAIN
    main.cpp
)

add_executable(
    ${EXECUTABLE_NAME}
    ${QT_DEPS}
    ${DEPENDENCIES}
)
target_compile_options(
    ${EXECUTABLE_NAME}
    PRIVATE -Wall -Wextra
)
target_compile_features(
    ${EXECUTABLE_NAME}
    PRIVATE cxx_std_17
)
target_include_directories(
    ${EXECUTABLE_NAME}
    PRIVATE ${Qt5Widgets_INCLUDE_DIRS}
    PRIVATE ${CMAKE_SOURCE_DIR}/include/
)
target_link_libraries(
    ${EXECUTABLE_NAME}
    PRIVATE ${Qt5Widgets_LIBRARIES}
)
