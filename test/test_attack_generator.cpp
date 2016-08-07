/*
 * test_attack_generator.cpp
 *
 *  Created on: 07.08.2016
 *      Author: mike
 */

#include "attack_generator.hpp"
#include "io.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_attack_generator)

const std::initializer_list<node_t> nodes
{
	node_t(node_t::INITIAL_POSITION),
	node_t("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 1 1")
};

BOOST_AUTO_TEST_SUITE(test_white)

const std::initializer_list<board_t> boards
{
	(R1 | R2 | R3) & ~(A1 | H1),
	(R1 | R2 | R3 | A6 | B5 | C4 | D5 | F5 | G4 | H5) & ~(A1 | H1)
};

BOOST_DATA_TEST_CASE(test, nodes ^ boards, node, board)
{
	const board_t attack = attack_generator::generate<white_tag>(node);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(board));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_black)

const std::initializer_list<board_t> boards
{
	(R8 | R7 | R6) & ~(A8 | H8),
	(R8 | R7 | R6) & ~(A8 | H8)
};

BOOST_DATA_TEST_CASE(test, nodes ^ boards, node, board)
{
	const board_t attack = attack_generator::generate<black_tag>(node);
	BOOST_CHECK_EQUAL(std::bitset<64>(attack), std::bitset<64>(board));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}
