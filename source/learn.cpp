/*
 * pgn_parser.cpp
 *
 *  Created on: 17.10.2017
 *      Author: mgresens
 */

//#define BOOST_SPIRIT_X3_DEBUG

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <iostream>

namespace x3 = boost::spirit::x3;

namespace pgn {
namespace ast {

typedef std::optional<float> score_t;
typedef std::tuple<std::string, score_t> move_t;
typedef std::vector<move_t> game_t;

}
namespace parser {

x3::rule<struct game, ast::game_t> const game("game");
x3::rule<struct move, ast::move_t> const move("move");
x3::rule<struct identifier, std::string> const identifier("identifier");
x3::rule<struct rank, char> const rank("rank");
x3::rule<struct file, char> const file("file");
x3::rule<struct comment, ast::score_t> const comment("comment");
x3::rule<struct skip> const skip("skip");

auto const game_def = *move;
auto const move_def = identifier >> -comment;
auto const identifier_def = file >> rank >> file >> rank >> -x3::char_("QRBN") >> x3::omit[-x3::char_("+#")];
auto const rank_def = x3::char_("1-8");
auto const file_def = x3::char_("a-h");
auto const comment_def = x3::lit('{') >> -x3::float_ >> x3::omit[*(x3::char_ - x3::lit('}'))] >> x3::lit('}');
auto const skip_def = x3::lit('(') >> *(x3::char_ - x3::lit(')')) >> x3::lit(')');

BOOST_SPIRIT_DEFINE(game, move, identifier, rank, file, comment, skip);

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
		auto const game = pgn::parse(line);
		std::cout << game.size() << ' ';
//		for (auto const& move : game)
//			std::cout << std::get<0>(move) << ' ' << std::get<1>(move).value_or(999) << ' ';
	}
	std::cout << std::endl;
}

int main() {
	try {
		parse(std::cin);
	}
	catch (std::exception const& exception) {
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}
