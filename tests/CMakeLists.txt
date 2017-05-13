if( Boost_FOUND )

	# include_directories(${Boost_INCLUDE_DIRS})  -> is already in root CMakeLists.txt
	add_executable( Test.exe
		../src/GameKI.cc
		../src/GamePanel.cc
		../src/Tree.cc
		TreeTest.cc
	)
	target_link_libraries( Test.exe ${Boost_LIBRARIES} )

endif()
 
