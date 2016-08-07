/*
 * test_attacker_rook.cpp
 *
 *  Created on: 03.08.2016
 *      Author: mike
 */

#include "attacker.hpp"
#include "io.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_attacker_rook)

constexpr std::initializer_list<board_t> boards
{
	A1,
	A1,
	A8,
	A8,
	H1,
	H1,
	H8,
	H8,
	D4 | E5
};

constexpr std::initializer_list<board_t> occupies
{
	A1,
	A1 | A3 | C1,
	A8,
	A8 | A6 | C8,
	H1,
	H1 | H3 | F1,
	H8,
	H8 | H6 | F8,
	D4 | E5 | D2 | D7 | B4 | G4 | E2 | E7 | B5 | G5
};

constexpr std::initializer_list<board_t> expectations
{
	A2 | A3 | A4 | A5 | A6 | A7 | A8 | B1 | C1 | D1 | E1 | F1 | G1 | H1,
	A2 | A3 | B1 | C1,
	A1 | A2 | A3 | A4 | A5 | A6 | A7 | B8 | C8 | D8 | E8 | F8 | G8 | H8,
	A7 | A6 | B8 | C8,
	H2 | H3 | H4 | H5 | H6 | H7 | H8 | A1 | B1 | C1 | D1 | E1 | F1 | G1,
	H2 | H3 | F1 | G1,
	H1 | H2 | H3 | H4 | H5 | H6 | H7 | A8 | B8 | C8 | D8 | E8 | F8 | G8,
	H7 | H6 | F8 | G8,
	D2 | D3 | D5 | D6 | D7 | B4 | C4 | E4 | F4 | G4 | E2 | E3 | E4 | E6 | E7 | B5 | C5 | D5 | F5 | G5
};

BOOST_DATA_TEST_CASE(test_white, boards ^ occupies ^ expectations, board, occupy, expectation)
{
	const board_t attack = detail::attacker<rook_queen_tag, white_tag>::attack(board, occupy);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_DATA_TEST_CASE(test_black, boards ^ occupies ^ expectations, board, occupy, expectation)
{
	const board_t attack = detail::attacker<rook_queen_tag, black_tag>::attack(board, occupy);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

}
