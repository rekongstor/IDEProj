#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>

#include "../include/field.hpp"

BOOST_AUTO_TEST_CASE(myTestCase)
{
	field test_f;
	BOOST_TEST(test_f.get_cell(0, 0) == 0);
	BOOST_TEST(test_f.get_cell(10, 0) == 0);
	BOOST_TEST(test_f.get_cell(0, 10) == 0);
	
	BOOST_TEST(test_f.set_cell(5, 5, 5) == true);
	BOOST_TEST(test_f.set_cell(10, 5, 5) == false);
	BOOST_TEST(test_f.set_cell(5, 10, 5) == false);
	BOOST_TEST(test_f.get_cell(5, 5) == 5);

	test_f.clear();
	BOOST_TEST(test_f.get_cell(5, 5) == 0);
}