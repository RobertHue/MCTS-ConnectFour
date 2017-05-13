if( Boost_FOUND )

	# include_directories(${Boost_INCLUDE_DIRS})  -> is already in root CMakeLists.txt
	add_executable( Test.exe
		../src/GameKI.cc ../includes/GameKI.h
		../src/GamePanel.cc ../includes/GamePanel.h
		../src/Tree.cc ../includes/Tree.h
		TreeTest.cc
	)
	target_link_libraries( Test.exe ${Boost_LIBRARIES} )

endif()

