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
//	node_t(node_t::INITIAL_POSITION),
//	node_t("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 1 1"),
//	node_t("r3k2r/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 1 1"),
//	node_t("rq2r1k1/5pp1/p7/4bNP1/1p2P2P/5Q2/PP4K1/5R1R w - - 0 1"),
	node_t("8/8/8/8/8/3p1p2/4P3/8 w - - 0 1"),
	node_t("8/8/5p2/3pP3/8/8/8/8 w - d6 0 1"),
	node_t("3p1p2/4P3/8/8/8/8/8/8 w - - 0 1"),
};

BOOST_AUTO_TEST_SUITE(test_active)

const std::initializer_list<std::initializer_list<move_t>> movess
{
//	{},
//	{},
//	{},
//	{{f5, g7}},
	{{e2, d3}, {e2, f3}},
	{{e5, d6}, {e5, f6}},
	{{e7, d8, Q}, {e7, d8, R}, {e7, d8, B}, {e7, d8, N}, {e7, f8, Q}, {e7, f8, R}, {e7, f8, B}, {e7, f8, N}, {e7, e8, Q}, {e7, e8, R}, {e7, e8, B}, {e7, e8, N}}
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
//	{{b1, a3}, {b1, c3}, {g1, f3}, {g1, h3}},
//	{{b8, a6}, {b8, c6}, {g8, f6}, {g8, h6}},
//	{{e8, d8}, {e8, f8}, {e8, g8}, {e8, c8}, {a8, b8}, {a8, c8}, {a8, d8}, {h8, f8}, {h8, g8}},
//	{
//		{g2, g1}, {g2, f2}, {g2, h3}, {f1, a1}, {f1, b1}, {f1, c1}, {f1, d1}, {f1, e1}, {f1, g1}, {f1, f2}, {h1, g1},
//		{h1, h2}, {h1, h3}, {f3, f2}, {f3, a3}, {f3, b3}, {f3, c3}, {f3, d3}, {f3, e3}, {f3, g3}, {f3, h3}, {f3, f4},
//		{f3, d1}, {f3, e2}, {f3, g4}, {f3, h5}, {f5, e3}, {f5, g3}, {f5, d4}, {f5, d6}, {f5, h6}, {f5, e7}, {f5, g7}
//	},
	{{e2, d3}, {e2, f3}, {e2, e3}, {e2, e4}},
	{{e5, d6}, {e5, f6}, {e5, e6}},
	{{e7, d8, Q}, {e7, d8, R}, {e7, d8, B}, {e7, d8, N}, {e7, f8, Q}, {e7, f8, R}, {e7, f8, B}, {e7, f8, N}, {e7, e8, Q}, {e7, e8, R}, {e7, e8, B}, {e7, e8, N}}
};

BOOST_DATA_TEST_CASE(test, nodes ^ movess, node, moves)
{
	const move_generator<all_tag> generator(node);
	BOOST_CHECK_EQUAL_COLLECTIONS(generator.begin(), generator.end(), moves.begin(), moves.end());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}
