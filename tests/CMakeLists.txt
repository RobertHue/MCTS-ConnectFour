if( Boost_FOUND )

	message(STATUS " ############ SETTING UP TEST MODULE ############ ")
	message(STATUS "boost-include-dirs : ${Boost_INCLUDE_DIRS}")
	message(STATUS "Boost_LIBRARIES : ${Boost_LIBRARIES}")
	
	message(STATUS "DEPENDENCIES : ${DEPENDENCIES}")
	add_executable( 
		${TEST_NAME} 
		${DEPENDENCIES} MCTS_Tests.cpp
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

