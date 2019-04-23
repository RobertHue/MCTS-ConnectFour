if( Boost_FOUND AND Qt5Widgets_FOUND )

	message(STATUS " ############ SETTING UP MAIN MODULE ############ ")
	message(STATUS "DEPENDENCIES : ${DEPENDENCIES}") 
	message(STATUS "boost-include-dirs : ${Boost_INCLUDE_DIRS}")
	message(STATUS "Boost_LIBRARIES : ${Boost_LIBRARIES}")
	
	# String describing the version of the module.
	message(STATUS "Qt5Widgets_VERSION : ${Qt5Widgets_VERSION}")
	# List of libraries for use with the target_link_libraries command.
	message(STATUS "Qt5Widgets_LIBRARIES : ${Qt5Widgets_LIBRARIES}")
	# List of directories for use with the include_directories command.
	message(STATUS "Qt5Widgets_INCLUDE_DIRS : ${Qt5Widgets_INCLUDE_DIRS}")
	# List of definitions for use with add_definitions.
	message(STATUS "Qt5Widgets_DEFINITIONS : ${Qt5Widgets_DEFINITIONS}")
	# List of definitions for use with the COMPILE_DEFINITIONS target property.
	message(STATUS "Qt5Widgets_COMPILE_DEFINITIONS : ${Qt5Widgets_COMPILE_DEFINITIONS}")
	# Boolean describing whether the module was found successfully.
	message(STATUS "Qt5Widgets_FOUND : ${Qt5Widgets_FOUND}")
	# String of flags to be used when building executables.
	message(STATUS "Qt5Widgets_EXECUTABLE_COMPILE_FLAGS : ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	
	message(STATUS "QT_DEPS : ${QT_DEPS}")
	
	add_executable( 
		${EXECUTABLE_NAME} 
		${DEPENDENCIES} 
		${QT_DEPS}
		main.cpp
	)
	target_compile_features(
		${EXECUTABLE_NAME} 
		PRIVATE cxx_std_14
	)
	target_include_directories(
		${EXECUTABLE_NAME}
		PRIVATE ../qt/
		PRIVATE ${Boost_INCLUDE_DIRS}
		PRIVATE ${Qt5Widgets_INCLUDE_DIRS}
	)
	target_link_libraries( 
		${EXECUTABLE_NAME} 
		PRIVATE ${Boost_LIBRARIES} 
		PRIVATE ${Qt5Widgets_LIBRARIES}
	)
	
elseif( NOT Boost_FOUND)
	message(STATUS "cannot find Boost!!!!!!!!!!!")
elseif( NOT Qt5Widgets_FOUND)
	message(STATUS "cannot find Qt5Widgets!!!!!!!!!!!")
endif()
