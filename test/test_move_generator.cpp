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

history_table_t h_table, b_table;
killer_table_t::entry_t k_entry;

const std::initializer_list<node_t> nodes
{
	node_t(node_t::INITIAL_POSITION),
	node_t("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 1 1"),
	node_t("r3k2r/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 1 1"),
	node_t("rq2r1k1/5pp1/p7/4bNP1/1p2P2P/5Q2/PP4K1/5R1R w - - 0 1"),

	node_t("8/8/8/8/8/3p1p2/4P3/8 w - - 0 1"),
	node_t("8/8/8/8/8/3ppp2/4P3/8 w - - 0 1"),
	node_t("8/8/8/8/4p3/3p1p2/4P3/8 w - - 0 1"),
	node_t("8/8/5p2/3pP3/8/8/8/8 w - d6 0 1"),
	node_t("8/8/4pp2/3pP3/8/8/8/8 w - d6 0 1"),
	node_t("3p1p2/4P3/8/8/8/8/8/8 w - - 0 1"),
	node_t("3ppp2/4P3/8/8/8/8/8/8 w - - 0 1"),

	node_t("8/4p3/3P1P2/8/8/8/8/8 b - - 0 1"),
	node_t("8/4p3/3PPP2/8/8/8/8/8 b - - 0 1"),
	node_t("8/4p3/3P1P2/4P3/8/8/8/8 b - - 0 1"),
	node_t("8/8/8/8/3Pp3/5P2/8/8 b - d3 0 1"),

	// rochade
	node_t("4k3/8/8/8/8/p2ppp1p/P2PPP1P/R3K2R w KQ - 0 1"),
	node_t("4k3/8/8/8/8/p2ppp1p/P2PPP1P/R3Kn1R w KQ - 0 1"),
	node_t("4k3/8/8/8/8/p2ppp1p/P2PPP1P/R3K1nR w KQ - 0 1"),
	node_t("4k3/8/8/8/8/p2ppp1p/P2PPP1P/R2nK2R w KQ - 0 1"),
	node_t("4k3/8/8/8/8/p2ppp1p/P2PPP1P/R1n1K2R w KQ - 0 1"),
	node_t("4k3/8/8/8/8/p2ppp1p/P2PPP1P/Rn2K2R w KQ - 0 1"),
	node_t("4k3/8/8/8/8/p2ppp1b/P2PPP1P/R3K2R w KQ - 0 1"),
	node_t("4k3/8/8/8/8/p2ppp1n/P2PPP1P/R3K2R w KQ - 0 1"),
	node_t("4k3/8/8/8/8/p2ppb1p/P2PPP1P/R3K2R w KQ - 0 1"),
	node_t("4k3/8/8/8/8/p2ppp1p/P2PPPnP/R3K2R w KQ - 0 1"),
	node_t("4k3/8/8/8/3b4/p2ppp1p/P2PPP1P/R3K2R w KQ - 0 1"),
	node_t("4k3/8/8/8/8/n2ppp1p/P2PPP1P/R3K2R w KQ - 0 1"),
	node_t("4k3/8/8/8/8/b2ppp1p/P2PPP1P/R3K2R w KQ - 0 1"),
	node_t("4k3/8/8/8/b7/p2ppp1p/P2PPP1P/R3K2R w KQ - 0 1"),
};

BOOST_AUTO_TEST_SUITE(test_active)

const std::initializer_list<std::initializer_list<move_t>> movess
{
	{},
	{},
	{},
	{{f5, g7}},

	{{e2, d3}, {e2, f3}},
	{{e2, d3}, {e2, f3}},
	{{e2, d3}, {e2, f3}},
	{{e5, d6}, {e5, f6}},
	{{e5, d6}, {e5, f6}},
	{{e7, d8, move_t::Q}, {e7, d8, move_t::R}, {e7, d8, move_t::B}, {e7, d8, move_t::N}, {e7, f8, move_t::Q}, {e7, f8, move_t::R}, {e7, f8, move_t::B}, {e7, f8, move_t::N}, {e7, e8, move_t::Q}, {e7, e8, move_t::R}, {e7, e8, move_t::B}, {e7, e8, move_t::N}},
	{{e7, d8, move_t::Q}, {e7, d8, move_t::R}, {e7, d8, move_t::B}, {e7, d8, move_t::N}, {e7, f8, move_t::Q}, {e7, f8, move_t::R}, {e7, f8, move_t::B}, {e7, f8, move_t::N}},

	{{e7, f6}, {e7, d6}},
	{{e7, f6}, {e7, d6}},
	{{e7, f6}, {e7, d6}},
	{{e4, f3}, {e4, d3}},

	// rochade
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, f1}, {h1, f1}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {h1, g1}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, d1}, {a1, d1}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {a1, c1}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {a1, b1}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}},
	{{d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}},
};

BOOST_DATA_TEST_CASE(test, nodes ^ movess, node, moves)
{
	const move_generator<active_tag> generator(node, h_table, b_table);
//	BOOST_CHECK_EQUAL_COLLECTIONS(generator.begin(), generator.end(), moves.begin(), moves.end());
	BOOST_CHECK(std::is_permutation(generator.begin(), generator.end(), moves.begin(), moves.end()));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_all)

const std::initializer_list<std::initializer_list<move_t>> movess
{
	{
		{b1, a3}, {b1, c3}, {g1, f3}, {g1, h3},
		{a2, a3}, {a2, a4}, {b2, b3}, {b2, b4}, {c2, c3}, {c2, c4}, {d2, d3}, {d2, d4},
		{e2, e3}, {e2, e4}, {f2, f3}, {f2, f4}, {g2, g3}, {g2, g4}, {h2, h3}, {h2, h4}
	},
	{
		{b8, a6}, {b8, c6}, {g8, f6}, {g8, h6},
		{a7, a6}, {a7, a5}, {b7, b6}, {b7, b5}, {c7, c6}, {c7, c5}, {d7, d6}, {d7, d5},
		{e7, e6}, {e7, e5}, {f7, f6}, {f7, f5}, {g7, g6}, {g7, g5}, {h7, h6}, {h7, h5}
	},
	{
		{e8, d8}, {e8, f8}, {e8, g8}, {e8, c8}, {a8, b8}, {a8, c8}, {a8, d8}, {h8, f8}, {h8, g8},
		{a7, a6}, {a7, a5}, {b7, b6}, {b7, b5}, {c7, c6}, {c7, c5}, {d7, d6}, {d7, d5},
		{e7, e6}, {e7, e5}, {f7, f6}, {f7, f5}, {g7, g6}, {g7, g5}, {h7, h6}, {h7, h5}
	},
	{
		{g2, g1}, {g2, f2}, {g2, h3}, {f1, a1}, {f1, b1}, {f1, c1}, {f1, d1}, {f1, e1}, {f1, g1}, {f1, f2}, {h1, g1},
		{h1, h2}, {h1, h3}, {f3, f2}, {f3, a3}, {f3, b3}, {f3, c3}, {f3, d3}, {f3, e3}, {f3, g3}, {f3, h3}, {f3, f4},
		{f3, d1}, {f3, e2}, {f3, g4}, {f3, h5}, {f5, e3}, {f5, g3}, {f5, d4}, {f5, d6}, {f5, h6}, {f5, e7}, {f5, g7},
		{a2, a3}, {a2, a4}, {b2, b3}, {g5, g6}, {h4, h5}
	},

	{{e2, d3}, {e2, f3}, {e2, e3}, {e2, e4}},
	{{e2, d3}, {e2, f3}},
	{{e2, d3}, {e2, f3}, {e2, e3}},
	{{e5, d6}, {e5, f6}, {e5, e6}},
	{{e5, d6}, {e5, f6}},
	{{e7, d8, move_t::Q}, {e7, d8, move_t::R}, {e7, d8, move_t::B}, {e7, d8, move_t::N}, {e7, f8, move_t::Q}, {e7, f8, move_t::R}, {e7, f8, move_t::B}, {e7, f8, move_t::N}, {e7, e8, move_t::Q}, {e7, e8, move_t::R}, {e7, e8, move_t::B}, {e7, e8, move_t::N}},
	{{e7, d8, move_t::Q}, {e7, d8, move_t::R}, {e7, d8, move_t::B}, {e7, d8, move_t::N}, {e7, f8, move_t::Q}, {e7, f8, move_t::R}, {e7, f8, move_t::B}, {e7, f8, move_t::N}},

	{{e7, f6}, {e7, d6}, {e7, e6}, {e7, e5}},
	{{e7, f6}, {e7, d6}},
	{{e7, f6}, {e7, d6}, {e7, e6}},
	{{e4, f3}, {e4, d3}, {e4, e3}},

	// rochade
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, c1}, {e1, g1}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, c1}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, c1}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, g1}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, g1}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, g1}},
	{{e1, d1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, c1}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, c1}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, c1}, {e1, g1}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, c1}, {e1, g1}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, c1}, {e1, g1}},
	{{e1, d1}, {e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, g1}},
	{{e1, f1}, {a1, b1}, {a1, c1}, {a1, d1}, {h1, g1}, {h1, f1}, {d2, e3}, {e2, d3}, {e2, f3}, {f2, e3}, {e1, g1}},
};

BOOST_DATA_TEST_CASE(test, nodes ^ movess, node, moves)
{
	const move_generator<all_tag> generator(node, h_table, b_table, k_entry, k_entry);
//	BOOST_CHECK_EQUAL_COLLECTIONS(generator.begin(), generator.end(), moves.begin(), moves.end());
	BOOST_CHECK(std::is_permutation(generator.begin(), generator.end(), moves.begin(), moves.end()));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}
