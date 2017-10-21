/*
 * pgn_parser.cpp
 *
 *  Created on: 17.10.2017
 *      Author: mgresens
 */

//#define BOOST_SPIRIT_X3_DEBUG

#include "node.hpp"
#include "move.hpp"
#include "io.hpp"
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <iostream>

namespace x3 = boost::spirit::x3;

BOOST_FUSION_ADAPT_STRUCT(chess::move_t, from, to, promotion);

namespace chess {
namespace pgn {
namespace ast {

typedef std::optional<float> score_t;
typedef std::tuple<move_t, score_t> entry_t;
typedef std::vector<entry_t> game_t;

}
namespace action {

const auto square = [](auto& ctx)
{
	using boost::fusion::at_c;
	auto& attr = x3::_attr(ctx);
	x3::_val(ctx) = square_of(at_c<0>(attr) - 'a', at_c<1>(attr) - '1');
};

}
namespace parser {

x3::rule<struct game, ast::game_t> const game("game");
x3::rule<struct entry, ast::entry_t> const entry("entry");
x3::rule<struct move, move_t> const move("move");
x3::rule<struct square_, square_t> const square("square");
x3::rule<struct promotion, move_t::promotion_t> const promotion("promotion");
x3::rule<struct score, ast::score_t> const score("score");
x3::rule<struct skip> const skip("skip");

auto const game_def = *entry;
auto const entry_def = move >> -score;
auto const move_def = square >> square >> -promotion >> x3::omit[-x3::char_("+#")];
auto const square_def = (x3::char_("a-h") >> x3::char_("1-8")) [action::square];
auto const promotion_def = x3::symbols<move_t::promotion_t>{{"Q", move_t::Q}, {"R", move_t::R}, {"B", move_t::B}, {"N", move_t::N}};
auto const score_def = x3::lit('{') >> -x3::float_ >> x3::omit[*(x3::char_ - x3::lit('}'))] >> x3::lit('}');
auto const skip_def = x3::lit('(') >> *(x3::char_ - x3::lit(')')) >> x3::lit(')');

BOOST_SPIRIT_DEFINE(game, entry, move, square, promotion, score, skip);

}

ast::game_t parse(std::string const& line) {
	ast::game_t game;
	auto begin = std::begin(line), end = std::end(line);
	auto const parsed = x3::phrase_parse(begin, end, parser::game, x3::space | parser::skip, game);
	if (!parsed || begin != end)
		throw std::runtime_error("failed: " + line);
	return game;
}

}

void parse(std::istream& file) {
	file.unsetf(std::ios::skipws);
	std::string line;
	while (std::getline(file, line)) {
		if (line.empty())
			continue;
		auto node = std::make_unique<node_t>(node_t::INITIAL_POSITION);
		for (auto const entry : pgn::parse(line)) {
			auto const& move = std::get<0>(entry);
			auto const& score = std::get<1>(entry);
			node = std::make_unique<node_t>(*node, move);
			std::cout << move << *node << score.value_or(999) << std::endl;
		}
	}
}

}

int main() {
	try {
		chess::parse(std::cin);
	}
	catch (std::exception const& exception) {
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}
