/*
 * test_attacker_knight.cpp
 *
 *  Created on: 03.08.2016
 *      Author: mike
 */

#include "attacker.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_attacker_knight)

constexpr std::initializer_list<board_t> boards
{
	A1,
	A8,
	H1,
	H8,
	B1 | G1,
	B8 | G8,
	D4 | E4
};

constexpr std::initializer_list<board_t> expectations
{
	B3 | C2,
	B6 | C7,
	F2 | G3,
	F7 | G6,
	A3 | C3 | D2 | E2 | F3 | H3,
	A6 | C6 | D7 | E7 | F6 | H6,
	B3 | B5 | C2 | C3 | C5 | C6 | D2 | D6 | E2 | E6 | F2 | F3 | F5 | F6 | G3 | G5
};

BOOST_DATA_TEST_CASE(test_white, boards ^ expectations, board, expectation)
{
	constexpr const node_t node {};
	const board_t attack = attacker<knight_tag, white_tag>::attack(node, board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_DATA_TEST_CASE(test_black, boards ^ expectations, board, expectation)
{
	constexpr const node_t node {};
	const board_t attack = attacker<knight_tag, black_tag>::attack(node, board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

}
