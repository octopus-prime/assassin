/*
 * test_attacker.cpp
 *
 *  Created on: 27.05.2016
 *      Author: mike_gresens
 */

#include "attacker.hpp"
#include "io.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_attacker)

BOOST_AUTO_TEST_SUITE(test_king)

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
	const board_t attack = detail::attacker<king_tag, white_tag>::attack(board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_DATA_TEST_CASE(test_black, boards ^ expectations, board, expectation)
{
	const board_t attack = detail::attacker<king_tag, black_tag>::attack(board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_rook_queen)

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

BOOST_AUTO_TEST_SUITE(test_bishop_queen)

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
	const board_t attack = detail::attacker<bishop_queen_tag, white_tag>::attack(board, occupy);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_DATA_TEST_CASE(test_black, boards ^ occupies ^ expectations, board, occupy, expectation)
{
	const board_t attack = detail::attacker<bishop_queen_tag, black_tag>::attack(board, occupy);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_knight)

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
	const board_t attack = detail::attacker<knight_tag, white_tag>::attack(board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_DATA_TEST_CASE(test_black, boards ^ expectations, board, expectation)
{
	const board_t attack = detail::attacker<knight_tag, black_tag>::attack(board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_pawn_white)

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
	const board_t attack = detail::attacker<pawn_tag, white_tag>::attack(board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_pawn_black)

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
	const board_t attack = detail::attacker<pawn_tag, black_tag>::attack(board);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(expectation));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}
