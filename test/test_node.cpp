/*
 * test_node.cpp
 *
 *  Created on: 24.05.2016
 *      Author: mike_gresens
 */

#include "node.hpp"
#include <boost/test/unit_test.hpp>

namespace chess {

BOOST_AUTO_TEST_SUITE(test_node_occupy)

constexpr auto white = E1 | E4 | C2 | D4 | E6;
constexpr auto black = E8 | F5 | C7;
constexpr node_t node {white, black};

BOOST_AUTO_TEST_CASE(test_white)
{
	BOOST_CHECK_EQUAL(node.occupy<white_tag>(), white);
}

BOOST_AUTO_TEST_CASE(test_black)
{
	BOOST_CHECK_EQUAL(node.occupy<black_tag>(), black);
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(node.occupy(), white | black);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_king)

constexpr auto white = E1;
constexpr auto black = E8;
constexpr node_t node {white, black, 0, 0, 0, 0, {}, e1, e8};

BOOST_AUTO_TEST_CASE(test_white)
{
	BOOST_CHECK_EQUAL(node.king<white_tag>(), e1);
	const board_t occupy1 = node.occupy<white_tag, king_tag>();
	BOOST_CHECK_EQUAL(occupy1, white);
	const board_t occupy2 = node.occupy<king_tag, white_tag>();
	BOOST_CHECK_EQUAL(occupy2, white);
}

BOOST_AUTO_TEST_CASE(test_black)
{
	BOOST_CHECK_EQUAL(node.king<black_tag>(), e8);
	const board_t occupy1 = node.occupy<black_tag, king_tag>();
	BOOST_CHECK_EQUAL(occupy1, black);
	const board_t occupy2 = node.occupy<king_tag, black_tag>();
	BOOST_CHECK_EQUAL(occupy2, black);
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(node.occupy<king_tag>(), white | black);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_rook)

constexpr auto white = A1 | H1;
constexpr auto black = A8 | H8;
constexpr node_t node {white, black, white | black};

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, rook_tag>();
	BOOST_CHECK_EQUAL(occupy1, white);
	const board_t occupy2 = node.occupy<rook_tag, white_tag>();
	BOOST_CHECK_EQUAL(occupy2, white);
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, rook_tag>();
	BOOST_CHECK_EQUAL(occupy1, black);
	const board_t occupy2 = node.occupy<rook_tag, black_tag>();
	BOOST_CHECK_EQUAL(occupy2, black);
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(node.occupy<rook_tag>(), white | black);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_bishop)

constexpr auto white = C1 | F1;
constexpr auto black = C8 | F8;
constexpr node_t node {white, black, 0, white | black};

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, bishop_tag>();
	BOOST_CHECK_EQUAL(occupy1, white);
	const board_t occupy2 = node.occupy<bishop_tag, white_tag>();
	BOOST_CHECK_EQUAL(occupy2, white);
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, bishop_tag>();
	BOOST_CHECK_EQUAL(occupy1, black);
	const board_t occupy2 = node.occupy<bishop_tag, black_tag>();
	BOOST_CHECK_EQUAL(occupy2, black);
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(node.occupy<bishop_tag>(), white | black);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_knight)

constexpr auto white = B1 | G1;
constexpr auto black = B8 | G8;
constexpr node_t node {white, black, 0, 0, white | black};

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, knight_tag>();
	BOOST_CHECK_EQUAL(occupy1, white);
	const board_t occupy2 = node.occupy<knight_tag, white_tag>();
	BOOST_CHECK_EQUAL(occupy2, white);
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, knight_tag>();
	BOOST_CHECK_EQUAL(occupy1, black);
	const board_t occupy2 = node.occupy<knight_tag, black_tag>();
	BOOST_CHECK_EQUAL(occupy2, black);
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(node.occupy<knight_tag>(), white | black);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_node_pawn)

constexpr auto white = R2;
constexpr auto black = R7;
constexpr node_t node {white, black, 0, 0, 0, white | black};

BOOST_AUTO_TEST_CASE(test_white)
{
	const board_t occupy1 = node.occupy<white_tag, pawn_tag>();
	BOOST_CHECK_EQUAL(occupy1, white);
	const board_t occupy2 = node.occupy<pawn_tag, white_tag>();
	BOOST_CHECK_EQUAL(occupy2, white);
}

BOOST_AUTO_TEST_CASE(test_black)
{
	const board_t occupy1 = node.occupy<black_tag, pawn_tag>();
	BOOST_CHECK_EQUAL(occupy1, black);
	const board_t occupy2 = node.occupy<pawn_tag, black_tag>();
	BOOST_CHECK_EQUAL(occupy2, black);
}

BOOST_AUTO_TEST_CASE(test_both)
{
	BOOST_CHECK_EQUAL(node.occupy<pawn_tag>(), white | black);
}

BOOST_AUTO_TEST_SUITE_END()

}
