/*
 * test_attacker_pawn.cpp
 *
 *  Created on: 03.08.2016
 *      Author: mike
 */

#include "attacker.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_attacker_pawn_white)

constexpr std::initializer_list<board_t> boards
{
	B2,
	B2 | C3 | D4,
};

constexpr std::initializer_list<board_t> expectations
{
	A3 | C3,
	A3 | C3 | B4 | D4 | C5 | E5,
};

BOOST_DATA_TEST_CASE(test, boards ^ expectations, board, expectation)
{
	constexpr node_t node {};
	const board_t attack = attacker<pawn_tag, white_tag>::attack(node, board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_attacker_pawn_black)

constexpr std::initializer_list<board_t> boards
{
	B7,
	B7 | C6 | D5,
};

constexpr std::initializer_list<board_t> expectations
{
	A6 | C6,
	A6 | C6 | B5 | D5 | C4 | E4,
};

BOOST_DATA_TEST_CASE(test, boards ^ expectations, board, expectation)
{
	constexpr node_t node {};
	const board_t attack = attacker<pawn_tag, black_tag>::attack(node, board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

}
