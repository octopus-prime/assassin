/*
 * test_move_generator.cpp
 *
 *  Created on: 08.08.2016
 *      Author: mike
 */

#include "move_generator.hpp"
#include "io.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace std {

ostream&
operator<<(ostream& stream, const initializer_list<chess::move_t>& moves)
{
	return stream;
}

}

namespace chess {

BOOST_AUTO_TEST_SUITE(test_move_generator)

const std::initializer_list<node_t> nodes
{
	node_t(node_t::INITIAL_POSITION),
	node_t("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 1 1")
};

BOOST_AUTO_TEST_SUITE(test_active)

const std::initializer_list<std::initializer_list<move_t>> movess
{
	{},
	{}
};

BOOST_DATA_TEST_CASE(test, nodes ^ movess, node, moves)
{
	const move_generator<active_tag> generator(node);
	BOOST_CHECK_EQUAL_COLLECTIONS(generator.begin(), generator.end(), moves.begin(), moves.end());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_all)

const std::initializer_list<std::initializer_list<move_t>> movess
{
	{{b1, a3}, {b1, c3}, {g1, f3}, {g1, h3}},
	{{b8, a6}, {b8, c6}, {g8, f6}, {g8, h6}}
};

BOOST_DATA_TEST_CASE(test, nodes ^ movess, node, moves)
{
	const move_generator<all_tag> generator(node);
	BOOST_CHECK_EQUAL_COLLECTIONS(generator.begin(), generator.end(), moves.begin(), moves.end());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}
