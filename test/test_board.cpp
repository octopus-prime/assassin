/*
 * test_board.cpp
 *
 *  Created on: 07.08.2016
 *      Author: mike
 */

#include "board.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace std {

ostream&
operator<<(ostream& stream, const initializer_list<chess::square_t>& squares)
{
	return stream;
}

}

namespace chess {

BOOST_AUTO_TEST_SUITE(test_board)

constexpr std::initializer_list<board_t> boards
{
	0,
	A1,
	H8,
	A1 | C2 | D3 | F5 | G7 | H8,
	~0UL
};

BOOST_AUTO_TEST_SUITE(test_bsf)

constexpr std::initializer_list<std::initializer_list<square_t>> squaress
{
	{},
	{a1},
	{h8},
	{a1, c2, d3, f5, g7, h8},
	{
		a1, b1, c1, d1, e1, f1, g1, h1,
		a2, b2, c2, d2, e2, f2, g2, h2,
		a3, b3, c3, d3, e3, f3, g3, h3,
		a4, b4, c4, d4, e4, f4, g4, h4,
		a5, b5, c5, d5, e5, f5, g5, h5,
		a6, b6, c6, d6, e6, f6, g6, h6,
		a7, b7, c7, d7, e7, f7, g7, h7,
		a8, b8, c8, d8, e8, f8, g8, h8
	}
};

BOOST_DATA_TEST_CASE(test, boards ^ squaress, board, squares)
{
	auto square = squares.begin();
	for (const square_t bit : bsf(board))
		BOOST_CHECK_EQUAL(bit, *square++);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_popcnt)

constexpr std::initializer_list<std::size_t> sizes
{
	0,
	1,
	1,
	6,
	64
};

BOOST_DATA_TEST_CASE(test, boards ^ sizes, board, size)
{
	BOOST_CHECK_EQUAL(popcnt(board), size);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}
