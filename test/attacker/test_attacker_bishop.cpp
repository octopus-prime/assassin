/*
 * test_attacker_bishop.cpp
 *
 *  Created on: 03.08.2016
 *      Author: mike
 */

#include "attacker.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_attacker_bishop)

constexpr std::initializer_list<board_t> boards
{
	C1,
	C1,
	C8,
	C8,
	F1,
	F1,
	F8,
	F8,
	D4 | E4
};

constexpr std::initializer_list<board_t> occupies
{
	C1,
	C1 | B2 | G5,
	C8,
	C8 | B7 | G4,
	F1,
	F1 | B5 | G2,
	F8,
	F8 | B4 | G7,
	D4 | E4 | B2 | B7 | C2 | C6 | G2 | G3 | G6 | G7
};

constexpr std::initializer_list<board_t> expectations
{
	A3 | B2 | D2 | E3 | F4 | G5 | H6,
	B2 | D2 | E3 | F4 | G5,
	A6 | B7 | D7 | E6 | F5 | G4 | H3,
	B7 | D7 | E6 | F5 | G4,
	A6 | B5 | C4 | D3 | E2 | G2 | H3,
	B5 | C4 | D3 | E2 | G2,
	A3 | B4 | C5 | D6 | E7 | G7 | H6,
	B4 | C5 | D6 | E7 | G7,
	A7 | B2 | B6 | C2 | C3 | C5 | C6 | D3 | D5 | E3 | E5 | F2 | F3 | F5 | F6 | G2 | G1 | G6 | G7
};

BOOST_DATA_TEST_CASE(test_white, boards ^ occupies ^ expectations, board, occupy, expectation)
{
	const node_t node {occupy, occupy};
	const board_t attack = attacker<bishop_tag, white_tag>::attack(node, board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_DATA_TEST_CASE(test_black, boards ^ occupies ^ expectations, board, occupy, expectation)
{
	const node_t node {occupy, occupy};
	const board_t attack = attacker<bishop_tag, black_tag>::attack(node, board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

}
