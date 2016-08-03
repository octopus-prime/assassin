/*
 * test_node.cpp
 *
 *  Created on: 24.05.2016
 *      Author: mike_gresens
 */

#include "node.hpp"
#include <boost/test/unit_test.hpp>

namespace test {

BOOST_AUTO_TEST_SUITE(test_node)

using namespace chess;

BOOST_AUTO_TEST_CASE(occupy)
{
	const node_t node {E1 | E4 | C2 | D4 | E6, E8 | F5 | C7};

	BOOST_CHECK_EQUAL(node.board_occupy(), E1 | E4 | C2 | D4 | E6 | E8 | F5 | C7);
	BOOST_CHECK_EQUAL(node.board_occupy<white_tag>(), E1 | E4 | C2 | D4 | E6);
	BOOST_CHECK_EQUAL(node.board_occupy<black_tag>(), E8 | F5 | C7);
}

BOOST_AUTO_TEST_CASE(king)
{
	const node_t node {E1, E8, 0, 0, 0, 0, {}, e1, e8};

	BOOST_CHECK_EQUAL(node.square_king<white_tag>(), e1);
	BOOST_CHECK_EQUAL(node.square_king<black_tag>(), e8);
	BOOST_CHECK_EQUAL(node.board_piece<king_tag>(), E1 | E8);
	BOOST_CHECK_EQUAL(node.board_piece<king_tag>() & node.board_occupy<white_tag>(), E1);
	BOOST_CHECK_EQUAL(node.board_piece<king_tag>() & node.board_occupy<black_tag>(), E8);
}

BOOST_AUTO_TEST_CASE(rook)
{
	const node_t node {A1 | H1, A8 | H8, A1 | H1 | A8 | H8};

	BOOST_CHECK_EQUAL(node.board_piece<rook_tag>(), A1 | H1 | A8 | H8);
	BOOST_CHECK_EQUAL(node.board_piece<rook_tag>() & node.board_occupy<white_tag>(), A1 | H1);
	BOOST_CHECK_EQUAL(node.board_piece<rook_tag>() & node.board_occupy<black_tag>(), A8 | H8);
}

BOOST_AUTO_TEST_SUITE_END()

}
