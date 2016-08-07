/*
 * test_node_creator.cpp
 *
 *  Created on: 07.08.2016
 *      Author: mike
 */

#include "attack_generator.hpp"
#include "io.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/format.hpp>
#include <memory>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_node_creator)

BOOST_AUTO_TEST_SUITE(test_illegal)

const std::initializer_list<std::string> fens
{
	"",
	"Xnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	"rnbqkbn/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	"rnbqkbnrr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	"rnbqkbnr/pppppppp/8/7/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP w KQkq - 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR X KQkq - 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w X - 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR KQkq - 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq X 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq d4 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e5 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - X 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 X"
};

BOOST_DATA_TEST_CASE(test, fens, fen)
{
	BOOST_CHECK_THROW(node_t node(fen), std::exception);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_color)

const std::initializer_list<std::string> fens {"w", "b"};
const std::initializer_list<color_t> colors {white,	black};

BOOST_DATA_TEST_CASE(test, fens ^ colors, fen, color)
{
	boost::format format("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR %s - - 0 1");
	std::unique_ptr<node_t> node;
	BOOST_REQUIRE_NO_THROW(node = std::make_unique<node_t>((format % fen).str()));
	BOOST_CHECK_EQUAL(node->color(), color);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_castle)

const std::initializer_list<std::string> fens
{
	"-",
	"KQkq",
	"KQk",
	"KQq",
	"KQ",
	"Kkq",
	"Kk",
	"Kq",
	"K",
	"Qkq",
	"Qk",
	"Qq",
	"Q",
	"kq",
	"k",
	"q"
};

const std::initializer_list<castle_t> castles
{
	0,
	white_castle | black_castle,
	white_castle | black_castle_king,
	white_castle | black_castle_queen,
	white_castle,
	white_castle_king | black_castle,
	white_castle_king | black_castle_king,
	white_castle_king | black_castle_queen,
	white_castle_king,
	white_castle_queen | black_castle,
	white_castle_queen | black_castle_king,
	white_castle_queen | black_castle_queen,
	white_castle_queen,
	black_castle,
	black_castle_king,
	black_castle_queen
};

BOOST_DATA_TEST_CASE(test, fens ^ castles, fen, castle)
{
	boost::format format("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w %s - 0 1");
	std::unique_ptr<node_t> node;
	BOOST_REQUIRE_NO_THROW(node = std::make_unique<node_t>((format % fen).str()));
	BOOST_CHECK_EQUAL(node->castle(), castle);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_en_passent)

const std::initializer_list<std::string> fens
{
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6"
};

const std::initializer_list<color_t> squares
{
	a3, b3, c3, d3, e3, f3, g3, h3,
	a6, b6, c6, d6, e6, f6, g6, h6
};

BOOST_DATA_TEST_CASE(test, fens ^ squares, fen, square)
{
	boost::format format("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - %s 0 1");
	std::unique_ptr<node_t> node;
	BOOST_REQUIRE_NO_THROW(node = std::make_unique<node_t>((format % fen).str()));
	BOOST_CHECK_EQUAL(node->en_passant(), square);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_count)

BOOST_DATA_TEST_CASE(test, data::xrange(1, 9), count)
{
	boost::format format("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - %u %u");
	std::unique_ptr<node_t> node;
	BOOST_REQUIRE_NO_THROW(node = std::make_unique<node_t>((format % count % count).str()));
	BOOST_CHECK_EQUAL(node->half_moves(), count);
	BOOST_CHECK_EQUAL(node->full_moves(), count);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}
