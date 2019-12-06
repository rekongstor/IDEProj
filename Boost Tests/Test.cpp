#define BOOST_TEST_MODULE mytests
#include <boost/test/included/unit_test.hpp>

#include "../include/field.hpp"

BOOST_AUTO_TEST_CASE(set_cell_cases)
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

BOOST_AUTO_TEST_CASE(set_ship_cases)
{
	field test_f;
	BOOST_TEST(test_f.set_ship(-1,0,4,'h') == false); // coordinates are out of bound
	BOOST_TEST(test_f.set_ship(-1, 0, 4, 'v') == false);
	BOOST_TEST(test_f.set_ship(0, -1, 4, 'h') == false);
	BOOST_TEST(test_f.set_ship(0, -1, 4, 'v') == false);

	BOOST_TEST(test_f.set_ship(9, 9, 2, 'h') == false); // ship is out of bound
	BOOST_TEST(test_f.set_ship(9, 9, 2, 'v') == false);
	BOOST_TEST(test_f.set_ship(7, 7, 4, 'v') == false);
	BOOST_TEST(test_f.set_ship(7, 7, 4, 'h') == false);

	BOOST_TEST(test_f.set_ship(0, 0, 0, 'h') == false); // wrong length
	BOOST_TEST(test_f.set_ship(0, 0, 5, 'h') == false);

	BOOST_TEST(test_f.set_ship(0, 0, 4, 'c') == false); // wrong direction
	BOOST_TEST(test_f.set_ship(0, 0, 4, 'r') == false);


	BOOST_TEST(test_f.set_ship(0, 0, 1, 'h') == true);
	BOOST_TEST(test_f.set_ship(0, 0, 1, 'h') == false);
	BOOST_TEST(test_f.set_ship(1, 1, 1, 'h') == false);
	BOOST_TEST(test_f.set_ship(2, 2, 1, 'h') == true);

	test_f.clear();

	BOOST_TEST(test_f.set_ship(5, 5, 4, 'h') == true);
	BOOST_TEST(test_f.set_ship(7, 3, 4, 'v') == false);
	BOOST_TEST(test_f.set_ship(7, 3, 4, 'h') == false);
	BOOST_TEST(test_f.set_ship(7, 3, 3, 'h') == true);
}