if( Boost_FOUND )

	message(STATUS "boost-include-dirs : ${Boost_INCLUDE_DIRS}")
	message(STATUS "Boost_LIBRARIES : ${Boost_LIBRARIES}")
	
	add_executable( 
		${TEST_NAME} 
		${DEPENDENCIES}
	)
	target_include_directories(
		${TEST_NAME} PUBLIC
		${Boost_INCLUDE_DIRS}
	)	
	target_link_libraries( 
		${TEST_NAME} 
		${Boost_LIBRARIES} 
	)

endif()

