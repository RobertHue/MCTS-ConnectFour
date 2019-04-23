if( Boost_FOUND )

	message(STATUS " ############ SETTING UP TEST MODULE ############ ")
	message(STATUS "boost-include-dirs : ${Boost_INCLUDE_DIRS}")
	message(STATUS "Boost_LIBRARIES : ${Boost_LIBRARIES}")
	
	message(STATUS "DEPENDENCIES : ${DEPENDENCIES}")
	
	
	add_executable( 
		${TEST_NAME} 
		${DEPENDENCIES} 
		MCTS_Tests.cpp
	)
	target_compile_features(
		${EXECUTABLE_NAME} 
		PRIVATE cxx_std_14
	)
	target_include_directories(
		${TEST_NAME}
		PRIVATE ${Boost_INCLUDE_DIRS}
	)	
	target_link_libraries( 
		${TEST_NAME}
		PRIVATE ${Boost_LIBRARIES} 
	)

elseif( NOT Boost_FOUND)
	message(STATUS "cannot find Boost!!!!!!!!!!!")
endif()

