/*
 * test_attacker_king.cpp
 *
 *  Created on: 03.08.2016
 *      Author: mike
 */

#include "attacker.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_attacker_king)

constexpr std::initializer_list<board_t> boards
{
	A1,
	A8,
	H1,
	H8,
	E4
};

constexpr std::initializer_list<board_t> expectations
{
	A2 | B1 | B2,
	A7 | B8 | B7,
	G1 | G2 | H2,
	G8 | G7 | H7,
	D3 | D4 | D5 | E3 | E5 | F3 | F4 | F5
};

BOOST_DATA_TEST_CASE(test_white, boards ^ expectations, board, expectation)
{
	constexpr const node_t node {};
	const board_t attack = attacker<king_tag, white_tag>::attack(node, board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_DATA_TEST_CASE(test_black, boards ^ expectations, board, expectation)
{
	constexpr const node_t node {};
	const board_t attack = attacker<king_tag, black_tag>::attack(node, board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

}
