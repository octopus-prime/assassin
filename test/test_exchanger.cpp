/*
 * test_exchanger.cpp
 *
 *  Created on: 18.08.2016
 *      Author: mike
 */

#include "exchanger.hpp"
#include "io.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_exchanger)

const std::initializer_list<node_t> nodes
{
	node_t("7k/p7/1p6/8/8/1Q6/8/7K w - - 0 1"),
	node_t("7k/p7/1p6/2P5/8/1Q6/8/7K w - - 0 1"),
	node_t("7k/p7/1p6/2P5/8/1Q6/8/7K w - - 0 1"),
	node_t("7k/p7/1p6/8/8/1Q6/1R6/7K w - - 0 1"),

	node_t("k7/8/5n2/3p4/8/2N2B2/8/K7 w - - 0 1"),
	node_t("k7/8/5n2/3p4/8/2N2B2/8/K7 w - - 0 1"),

	node_t("k7/8/8/3pRrRr/8/8/8/K7 w - - 0 1"),
	node_t("k7/8/8/3pRrRr/8/8/8/K7 w - - 0 1"),
	node_t("k7/8/8/3pRrRr/8/8/8/K7 w - - 0 1"),
	node_t("k7/8/8/3pRrRr/8/8/8/K7 w - - 0 1"),

	node_t("k7/7P/8/8/8/8/8/K7 w - - 0 1"),
	node_t("k7/7P/8/8/8/8/8/K7 w - - 0 1"),
	node_t("kr6/7P/8/8/8/8/8/K7 w - - 0 1"),
	node_t("kr4r1/7P/8/8/8/8/8/K7 w - - 0 1"),

	node_t("8/8/8/8/8/8/2kpK3/7R b - - 0 1"),
	node_t("8/8/3r4/8/8/8/2kpK3/7R b - - 0 1"),
};

const std::initializer_list<move_t> moves
{
	move_t {b3, b6},
	move_t {b3, b6},
	move_t {c5, b6},
	move_t {b3, b6},

	move_t {c3, d5},
	move_t {f3, d5},

	move_t {e5, d5},
	move_t {e5, f5},
	move_t {g5, f5},
	move_t {g5, h5},

	move_t {h7, h8, 4},
	move_t {h7, h8, 1},
	move_t {h7, h8, 4},
	move_t {h7, g8, 4},

	move_t {d2, d1, 4},
	move_t {d2, d1, 4},
};

const std::initializer_list<score_t> scores
{
	1_P - 1_Q,
	2_P - 1_Q,
	1_P,
	2_P - 1_Q,

	1_P,
	1_P - 1_B + 1_N,

	1_P - 1_R,
	1_R,
	1_R,
	1_R,

	1_Q - 1_P,
	1_N - 1_P,
	-1_P,
	1_R - 1_P,

	-1_P,
	1_R - 1_P,
};

BOOST_DATA_TEST_CASE(test, nodes ^ moves ^ scores, node, move, score)
{
	BOOST_CHECK_EQUAL(exchanger::evaluate(node, move), score);
}

BOOST_AUTO_TEST_SUITE_END()

}
