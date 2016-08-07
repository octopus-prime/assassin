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

BOOST_AUTO_TEST_SUITE(test_legal)

const std::initializer_list<std::string> fens
{
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - - 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b - e3 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w - d6 0 1",
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b - - 17 53",
};

const std::initializer_list<color_t> colors
{
	white,
	black,
	white,
	black,
	white,
	black
};

const std::initializer_list<castle_t> castles
{
	white_castle | black_castle,
	white_castle | black_castle,
	0,
	0,
	0,
	0
};

const std::initializer_list<square_t> squares
{
	0,
	0,
	0,
	e3,
	d6,
	0
};

const std::initializer_list<std::uint8_t> halfs
{
	0,
	0,
	0,
	0,
	0,
	17
};

const std::initializer_list<std::uint8_t> fulls
{
	1,
	1,
	1,
	1,
	1,
	53
};

/*
const std::initializer_list<board_t> boards
{
	(R8 | R7 | R6) & ~(A8 | H8),
	(R8 | R7 | R6) & ~(A8 | H8)
};
*/
BOOST_DATA_TEST_CASE(test, fens ^ colors ^ castles ^ squares ^ halfs ^ fulls, fen, color, castle, square, half, full)
{
	std::unique_ptr<node_t> node;
	BOOST_REQUIRE_NO_THROW(node = std::make_unique<node_t>(fen));
	BOOST_CHECK(node->parent() == nullptr);
	BOOST_CHECK_EQUAL(node->color(), color);
	BOOST_CHECK_EQUAL(node->castle(), castle);
	BOOST_CHECK_EQUAL(node->en_passant(), square);
	BOOST_CHECK_EQUAL(node->half_moves(), half);
	BOOST_CHECK_EQUAL(node->full_moves(), full);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}
