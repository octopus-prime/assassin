/*
 * test_analyzer.cpp
 *
 *  Created on: 27.08.2016
 *      Author: mike
 */

#include "analyzer.hpp"
#include "io.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_analyzer)

BOOST_AUTO_TEST_SUITE(test_isolated)

const std::initializer_list<node_t> nodes
{
	node_t("4k3/8/8/p1p1ppp1/P1P1PPP1/8/8/4K3 w - - 0 1"),
	node_t("4k3/8/8/p1p1ppp1/P1P1PPP1/8/8/4K3 w - - 0 1"),
	node_t("4k3/8/8/p1p1ppp1/P1P1PPP1/8/8/4K3 w - - 0 1"),
	node_t("4k3/8/8/p1p1ppp1/P1P1PPP1/8/8/4K3 w - - 0 1"),
	node_t("4k3/8/8/p1p1ppp1/P1P1PPP1/8/8/4K3 w - - 0 1"),
};

const std::initializer_list<bool> isolateds
{
	true,
	true,
	false,
	false,
	false
};

BOOST_AUTO_TEST_SUITE(test_white)

const std::initializer_list<square> squares
{
	a4,
	c4,
	e4,
	f4,
	g4,
};

BOOST_DATA_TEST_CASE(test, nodes ^ squares ^ isolateds, node, square, isolated)
{
	typedef detail::pawn_analyser_t<white_tag> analyzer_t;
	const analyzer_t analyze(node);
	BOOST_CHECK_EQUAL(analyzer_t::is_isolated(analyze(square)), isolated);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_black)

const std::initializer_list<square> squares
{
	a5,
	c5,
	e5,
	f5,
	g5,
};

BOOST_DATA_TEST_CASE(test, nodes ^ squares ^ isolateds, node, square, isolated)
{
	typedef detail::pawn_analyser_t<black_tag> analyzer_t;
	const analyzer_t analyze(node);
	BOOST_CHECK_EQUAL(analyzer_t::is_isolated(analyze(square)), isolated);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_doubled)

const std::initializer_list<node_t> nodes
{
	node_t("4k3/p1p1p2p/p1p5/2p5/2P5/P1P5/P1P1P2P/4K3 w - - 0 1"),
	node_t("4k3/p1p1p2p/p1p5/2p5/2P5/P1P5/P1P1P2P/4K3 w - - 0 1"),
	node_t("4k3/p1p1p2p/p1p5/2p5/2P5/P1P5/P1P1P2P/4K3 w - - 0 1"),
	node_t("4k3/p1p1p2p/p1p5/2p5/2P5/P1P5/P1P1P2P/4K3 w - - 0 1"),
	node_t("4k3/p1p1p2p/p1p5/2p5/2P5/P1P5/P1P1P2P/4K3 w - - 0 1"),
	node_t("4k3/p1p1p2p/p1p5/2p5/2P5/P1P5/P1P1P2P/4K3 w - - 0 1"),
	node_t("4k3/p1p1p2p/p1p5/2p5/2P5/P1P5/P1P1P2P/4K3 w - - 0 1"),
};

const std::initializer_list<bool> doubleds
{
	true,
	true,
	true,
	true,
	true,
	false,
	false
};

BOOST_AUTO_TEST_SUITE(test_white)

const std::initializer_list<square> squares
{
	a2,
	a3,
	c2,
	c3,
	c4,
	e2,
	h2
};

BOOST_DATA_TEST_CASE(test, nodes ^ squares ^ doubleds, node, square, doubled)
{
	typedef detail::pawn_analyser_t<white_tag> analyzer_t;
	const analyzer_t analyze(node);
	BOOST_CHECK_EQUAL(analyzer_t::is_doubled(analyze(square)), doubled);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_black)

const std::initializer_list<square> squares
{
	a7,
	a6,
	c7,
	c6,
	c5,
	e7,
	h7
};

BOOST_DATA_TEST_CASE(test, nodes ^ squares ^ doubleds, node, square, doubled)
{
	typedef detail::pawn_analyser_t<black_tag> analyzer_t;
	const analyzer_t analyze(node);
	BOOST_CHECK_EQUAL(analyzer_t::is_doubled(analyze(square)), doubled);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_passed)

const std::initializer_list<bool> passeds
{
	true,
	true,
	false,
	true,
	false,
	false,
	false,
	true,
	true,
	false
};

BOOST_AUTO_TEST_SUITE(test_white)

const std::initializer_list<node_t> nodes
{
	node_t("4k3/5pp1/8/8/8/2P5/PPP1PP1P/4K3 w - - 0 1"),
	node_t("4k3/5pp1/8/8/8/2P5/PPP1PP1P/4K3 w - - 0 1"),
	node_t("4k3/5pp1/8/8/8/2P5/PPP1PP1P/4K3 w - - 0 1"),
	node_t("4k3/5pp1/8/8/8/2P5/PPP1PP1P/4K3 w - - 0 1"),
	node_t("4k3/5pp1/8/8/8/2P5/PPP1PP1P/4K3 w - - 0 1"),
	node_t("4k3/5pp1/8/8/8/2P5/PPP1PP1P/4K3 w - - 0 1"),
	node_t("4k3/5pp1/8/8/8/2P5/PPP1PP1P/4K3 w - - 0 1"),
	node_t("4k3/8/8/7p/Pp1P2P1/4p3/8/4K3 w - - 0 1"),
	node_t("4k3/8/8/7p/Pp1P2P1/4p3/8/4K3 w - - 0 1"),
	node_t("4k3/8/8/7p/Pp1P2P1/4p3/8/4K3 w - - 0 1"),
};

const std::initializer_list<square> squares
{
	a2,
	b2,
	c2,
	c3,
	e2,
	f2,
	h2,
	a4,
	d4,
	g4
};

BOOST_DATA_TEST_CASE(test, nodes ^ squares ^ passeds, node, square, passed)
{
	typedef detail::pawn_analyser_t<white_tag> analyzer_t;
	const analyzer_t analyze(node);
	BOOST_CHECK_EQUAL(analyzer_t::is_passed(analyze(square)), passed);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_black)

const std::initializer_list<node_t> nodes
{
	node_t("4k3/ppp1pp1p/2p5/8/8/8/5PP1/4K3 w - - 0 1"),
	node_t("4k3/ppp1pp1p/2p5/8/8/8/5PP1/4K3 w - - 0 1"),
	node_t("4k3/ppp1pp1p/2p5/8/8/8/5PP1/4K3 w - - 0 1"),
	node_t("4k3/ppp1pp1p/2p5/8/8/8/5PP1/4K3 w - - 0 1"),
	node_t("4k3/ppp1pp1p/2p5/8/8/8/5PP1/4K3 w - - 0 1"),
	node_t("4k3/ppp1pp1p/2p5/8/8/8/5PP1/4K3 w - - 0 1"),
	node_t("4k3/ppp1pp1p/2p5/8/8/8/5PP1/4K3 w - - 0 1"),
	node_t("4k3/8/8/7p/Pp1P2P1/4p3/8/4K3 w - - 0 1"),
	node_t("4k3/8/8/7p/Pp1P2P1/4p3/8/4K3 w - - 0 1"),
	node_t("4k3/8/8/7p/Pp1P2P1/4p3/8/4K3 w - - 0 1"),
};

const std::initializer_list<square> squares
{
	a7,
	b7,
	c7,
	c6,
	e7,
	f7,
	h7,
	b4,
	e3,
	h5
};

BOOST_DATA_TEST_CASE(test, nodes ^ squares ^ passeds, node, square, passed)
{
	typedef detail::pawn_analyser_t<black_tag> analyzer_t;
	const analyzer_t analyze(node);
	BOOST_CHECK_EQUAL(analyzer_t::is_passed(analyze(square)), passed);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}
