#define BOOST_TEST_MODULE "TreeModule"
#include <boost/test/unit_test.hpp>
#include <iostream>

#include "Tree.h"


BOOST_AUTO_TEST_CASE( TreeFunctionality1 )
{
	BOOST_TEST_MESSAGE("TreeFunctionality1");

	Tree tree;
	BOOST_CHECK(tree.isEmpty() == false);
}
