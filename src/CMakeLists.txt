if( Boost_FOUND  )

	# include_directories(${Boost_INCLUDE_DIRS})  -> is already in root CMakeLists.txt
	add_executable( MCTS_ConnectFour.exe
		GameKI.cc ../includes/GameKI.h
		GamePanel.cc ../includes/GamePanel.h
		Tree.cc ../includes/Tree.h
		main.cc
	)
	target_link_libraries( MCTS_ConnectFour.exe ${Boost_LIBRARIES} )

endif()
