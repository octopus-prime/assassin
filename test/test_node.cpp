/*
 * test_node.cpp
 *
 *  Created on: 24.05.2016
 *      Author: mike_gresens
 */

#include "node.hpp"
#include "io.hpp"
#include <boost/test/unit_test.hpp>

namespace chess {

BOOST_AUTO_TEST_SUITE(test_node_occupy)

constexpr auto white = R1 | R2;
constexpr auto black = R7 | R8;
const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test_white)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy<white_tag>()), std::bitset<64>(white));
}

BOOST_AUTO_TEST_CASE(test_black)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy<black_tag>()), std::bitset<64>(black));
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy()), std::bitset<64>(white | black));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_king)

constexpr auto white = E1;
constexpr auto black = E8;
const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test_white)
{
	BOOST_CHECK_EQUAL(node.king<white_tag>(), e1);
	const board_t occupy1 = node.occupy<white_tag, king_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(white));
	const board_t occupy2 = node.occupy<king_tag, white_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(white));
}

BOOST_AUTO_TEST_CASE(test_black)
{
	BOOST_CHECK_EQUAL(node.king<black_tag>(), e8);
	const board_t occupy1 = node.occupy<black_tag, king_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(black));
	const board_t occupy2 = node.occupy<king_tag, black_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(black));
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy<king_tag>()), std::bitset<64>(white | black));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_queen)

constexpr auto white = D1;
constexpr auto black = D8;
const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, queen_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(white));
	const board_t occupy2 = node.occupy<queen_tag, white_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(white));
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, queen_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(black));
	const board_t occupy2 = node.occupy<queen_tag, black_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(black));
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy<queen_tag>()), std::bitset<64>(white | black));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_rook_queen)

constexpr auto white = A1 | D1 | H1;
constexpr auto black = A8 | D8 | H8;
const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, rook_queen_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(white));
	const board_t occupy2 = node.occupy<rook_queen_tag, white_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(white));
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, rook_queen_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(black));
	const board_t occupy2 = node.occupy<rook_queen_tag, black_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(black));
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy<rook_queen_tag>()), std::bitset<64>(white | black));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_rook)

constexpr auto white = A1 | H1;
constexpr auto black = A8 | H8;
const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, rook_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(white));
	const board_t occupy2 = node.occupy<rook_tag, white_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(white));
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, rook_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(black));
	const board_t occupy2 = node.occupy<rook_tag, black_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(black));
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy<rook_tag>()), std::bitset<64>(white | black));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_bishop_queen)

constexpr auto white = C1 | D1 | F1;
constexpr auto black = C8 | D8 | F8;
const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, bishop_queen_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(white));
	const board_t occupy2 = node.occupy<bishop_queen_tag, white_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(white));
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, bishop_queen_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(black));
	const board_t occupy2 = node.occupy<bishop_queen_tag, black_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(black));
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy<bishop_queen_tag>()), std::bitset<64>(white | black));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_bishop)

constexpr auto white = C1 | F1;
constexpr auto black = C8 | F8;
const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, bishop_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(white));
	const board_t occupy2 = node.occupy<bishop_tag, white_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(white));
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, bishop_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(black));
	const board_t occupy2 = node.occupy<bishop_tag, black_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(black));
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy<bishop_tag>()), std::bitset<64>(white | black));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_knight)

constexpr auto white = B1 | G1;
constexpr auto black = B8 | G8;
const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, knight_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(white));
	const board_t occupy2 = node.occupy<knight_tag, white_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(white));
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, knight_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(black));
	const board_t occupy2 = node.occupy<knight_tag, black_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(black));
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy<knight_tag>()), std::bitset<64>(white | black));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_pawn)

constexpr auto white = R2;
constexpr auto black = R7;
const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, pawn_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(white));
	const board_t occupy2 = node.occupy<pawn_tag, white_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(white));
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, pawn_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy1), std::bitset<64>(black));
	const board_t occupy2 = node.occupy<pawn_tag, black_tag>();
	BOOST_CHECK_EQUAL(std::bitset<64>(occupy2), std::bitset<64>(black));
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.occupy<pawn_tag>()), std::bitset<64>(white | black));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_attack)

constexpr auto white = (R1 | R2 | R3) & ~(A1 | H1);
constexpr auto black = (R8 | R7 | R6) & ~(A8 | H8);
const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test_white)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.attack<white_tag>()), std::bitset<64>(white));
}

BOOST_AUTO_TEST_CASE(test_black)
{
	BOOST_CHECK_EQUAL(std::bitset<64>(node.attack<black_tag>()), std::bitset<64>(black));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_flip)

const node_t node(node_t::INITIAL_POSITION);

BOOST_AUTO_TEST_CASE(test)
{
	const auto hash = node.hash();
	BOOST_CHECK_EQUAL(node.color(), white);
	const auto en_passant = const_cast<node_t&>(node).flip(0);
	BOOST_CHECK_EQUAL(node.color(), black);
	BOOST_CHECK_NE(node.hash(), hash);
	BOOST_CHECK_EQUAL(node.hash(), hash ^ color_hash);
	const_cast<node_t&>(node).flip(en_passant);
	BOOST_CHECK_EQUAL(node.color(), white);
	BOOST_CHECK_EQUAL(node.hash(), hash);
}

BOOST_AUTO_TEST_SUITE_END()

}
