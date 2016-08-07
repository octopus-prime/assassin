/*
 * node_creator.cpp
 *
 *  Created on: 06.08.2016
 *      Author: mike
 */

#include "node.hpp"
#include "attack_generator.hpp"
#include <unordered_map>
#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

namespace chess {

struct node_t::creator
{
	static void set_attack(node_t& node);
	static void set_hash(node_t& node);

	static void set_color(node_t& node, const color_t color);

	static void set_castle(node_t& node, const castle_t castle);

	static void set_en_passant(node_t& node, const square_t square);

	static void set_counts(node_t& node, const std::int8_t half, const std::int8_t full);

	template <piece_t piece>
	static void set_piece(node_t& node, const square_t square);

private:
	template <piece_t piece>
	static void set_mask(node_t& node, const board_t mask);
};

namespace parser {

struct node_tag;
struct rank_tag;
struct file_tag;

const auto fen_action = [](auto& ctx)
{
	node_t::creator::set_attack(x3::get<node_tag>(ctx));
	node_t::creator::set_hash(x3::get<node_tag>(ctx));
};

const auto rows_action = [](auto& ctx)
{
	if (x3::get<rank_tag>(ctx) != square_rank_t(-1))
		throw std::runtime_error("Need 8 ranks.");
};

const auto row_action = [](auto& ctx)
{
	if (x3::get<file_tag>(ctx) != 8)
		throw std::runtime_error("Need 8 files.");

	x3::get<file_tag>(ctx) -= 8;
	x3::get<rank_tag>(ctx)--;
};

const auto piece_action = [](auto& ctx)
{
	static const std::unordered_map<char, std::function<void (node_t&, const square_t)>> MAP
	{
		{'K', node_t::creator::set_piece<K>}, {'k', node_t::creator::set_piece<k>},
		{'Q', node_t::creator::set_piece<Q>}, {'q', node_t::creator::set_piece<q>},
		{'R', node_t::creator::set_piece<R>}, {'r', node_t::creator::set_piece<r>},
		{'B', node_t::creator::set_piece<B>}, {'b', node_t::creator::set_piece<b>},
		{'N', node_t::creator::set_piece<N>}, {'n', node_t::creator::set_piece<n>},
		{'P', node_t::creator::set_piece<P>}, {'p', node_t::creator::set_piece<p>}
	};

	const square_t square = square_of(x3::get<file_tag>(ctx)++, x3::get<rank_tag>(ctx));
	MAP.at(x3::_attr(ctx))(x3::get<node_tag>(ctx), square);
};

const auto empty_action = [](auto& ctx)
{
	for (char i = 0; i < (x3::_attr(ctx) - '0'); ++i)
	{
		const square_t square = square_of(x3::get<file_tag>(ctx)++, x3::get<rank_tag>(ctx));
		node_t::creator::set_piece<no_piece>(x3::get<node_tag>(ctx), square);
	}
};

const auto color_action = [](auto& ctx)
{
	node_t::creator::set_color(x3::get<node_tag>(ctx), x3::_attr(ctx));
};

const auto castle_action = [](auto& ctx)
{
	node_t::creator::set_castle(x3::get<node_tag>(ctx), x3::_attr(ctx));
};

const auto en_passant_action = [](auto& ctx)
{
	using boost::fusion::at_c;
	auto& attr = x3::_attr(ctx);
	const square_t square = square_of(at_c<0>(attr) - 'a', at_c<1>(attr) - '1');
	node_t::creator::set_en_passant(x3::get<node_tag>(ctx), square);
};

const auto counts_action = [](auto& ctx)
{
	using boost::fusion::at_c;
	auto& attr = x3::_attr(ctx);
	node_t::creator::set_counts(x3::get<node_tag>(ctx), at_c<0>(attr), at_c<1>(attr));
};

const x3::rule<class fen_rule> fen_rule ="fen";
const x3::rule<class rows_rule> rows_rule ="rows";
const x3::rule<class row_rule> row_rule ="row";
const x3::rule<class piece_rule> piece_rule ="piece";
const x3::rule<class empty_rule> empty_rule ="empty";
const x3::rule<class color_rule> color_rule ="color";
const x3::rule<class castles_rule> castles_rule ="castles";
const x3::rule<class castle_rule> castle_rule ="castle";
const x3::rule<class en_passant_rule> en_passant_rule ="en_passant";
const x3::rule<class square_rule> square_rule ="square";
const x3::rule<class counts_rule> counts_rule ="counts";

const auto fen_rule_def = (rows_rule >> x3::lit(' ') >> color_rule >> x3::lit(' ') >> castles_rule >> x3::lit(' ') >> en_passant_rule >> x3::lit(' ') >> counts_rule) [fen_action];
const auto rows_rule_def = (row_rule % x3::lit('/')) [rows_action];
const auto row_rule_def = (+(piece_rule | empty_rule)) [row_action];
const auto piece_rule_def = x3::char_("KQRBNPkqrbnp") [piece_action];
const auto empty_rule_def = x3::char_("1-8") [empty_action];
const auto color_rule_def = x3::symbols<color_t> {{"w", white}, {"b", black}} [color_action];
const auto castles_rule_def = x3::lit('-') | +castle_rule;
const auto castle_rule_def = x3::symbols<castle_t> {{"K", white_castle_king}, {"k", black_castle_king}, {"Q", white_castle_queen}, {"q", black_castle_queen}} [castle_action];
const auto en_passant_rule_def = x3::lit('-') | square_rule;
const auto square_rule_def = (x3::char_("a-h") >> x3::char_("36")) [en_passant_action];
const auto counts_rule_def = (x3::uint8 >> x3::lit(' ') >> x3::uint8) [counts_action];

BOOST_SPIRIT_DEFINE(fen_rule, rows_rule, row_rule, piece_rule, empty_rule, color_rule, castles_rule, castle_rule, en_passant_rule, square_rule, counts_rule);

}

const std::string
node_t::INITIAL_POSITION("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

node_t::node_t(const std::string& fen)
:
	node_t()
{
	auto iterator = fen.begin();

//	try
//	{
		square_rank_t rank = 7;
		square_file_t file = 0;
		const auto rule = x3::with<parser::node_tag>(std::ref(*this)) [ x3::with<parser::rank_tag>(std::ref(rank)) [ x3::with<parser::file_tag>(std::ref(file)) [x3::eps > parser::fen_rule] ] ];

		const bool ok = x3::parse(iterator, fen.end(), rule);

		if (!ok || iterator != fen.end())
			throw std::runtime_error("Parsing failed.");
//	}
//	catch(const x3::expectation_failure<char>& e)
//	{
//		throw std::runtime_error(e.what());
//	}
}

void node_t::creator::set_attack(node_t& node)
{
	node._attack_white = attack_generator::generate<white_tag>(node);
	node._attack_black = attack_generator::generate<black_tag>(node);
}

void node_t::creator::set_hash(node_t& node)
{
	// todo
}

void node_t::creator::set_color(node_t& node, const color_t color)
{
	node._color = color;
}

void node_t::creator::set_castle(node_t& node, const castle_t castle)
{
	node._castle |= castle;
}

void node_t::creator::set_en_passant(node_t& node, const square_t square)
{
	node._en_passant = square;
}

void node_t::creator::set_counts(node_t& node, const std::int8_t half, const std::int8_t full)
{
	node._half_moves = half;
	node._full_moves = full;
}

template <piece_t piece>
void
node_t::creator::set_piece(node_t& node, const square_t square)
{
	node._pieces.at(square) = piece;
	node._hash ^= piece_square_hash.at(piece).at(square);
	set_mask<piece>(node, board_of(square));
}

template <>
void node_t::creator::set_piece<no_piece>(node_t& node, const square_t square)
{
	node._pieces.at(square) = no_piece;
}

template <>
void node_t::creator::set_piece<K>(node_t& node, const square_t square)
{
	const board_t mask = board_of(square);
	node._occupy_white |= mask;
	node._king_white = square;
	node._pieces.at(square) = K;
	node._hash ^= piece_square_hash.at(K).at(square);
}

template <>
void node_t::creator::set_piece<k>(node_t& node, const square_t square)
{
	const board_t mask = board_of(square);
	node._occupy_black |= mask;
	node._king_black = square;
	node._pieces.at(square) = k;
	node._hash ^= piece_square_hash.at(k).at(square);
}

template <>
void node_t::creator::set_mask<Q>(node_t& node, const board_t mask)
{
	node._occupy_white |= mask;
	node._occupy_rook_queen |= mask;
	node._occupy_bishop_queen |= mask;
	node._score += 900;
}

template <>
void node_t::creator::set_mask<q>(node_t& node, const board_t mask)
{
	node._occupy_black |= mask;
	node._occupy_rook_queen |= mask;
	node._occupy_bishop_queen |= mask;
	node._score -= 900;
}

template <>
void node_t::creator::set_mask<R>(node_t& node, const board_t mask)
{
	node._occupy_white |= mask;
	node._occupy_rook_queen |= mask;
	node._score += 500;
}

template <>
void node_t::creator::set_mask<r>(node_t& node, const board_t mask)
{
	node._occupy_black |= mask;
	node._occupy_rook_queen |= mask;
	node._score -= 500;
}

template <>
void node_t::creator::set_mask<B>(node_t& node, const board_t mask)
{
	node._occupy_white |= mask;
	node._occupy_bishop_queen |= mask;
	node._score += 300;
}

template <>
void node_t::creator::set_mask<b>(node_t& node, const board_t mask)
{
	node._occupy_black |= mask;
	node._occupy_bishop_queen |= mask;
	node._score -= 300;
}

template <>
void node_t::creator::set_mask<N>(node_t& node, const board_t mask)
{
	node._occupy_white |= mask;
	node._occupy_knight |= mask;
	node._score += 300;
}

template <>
void node_t::creator::set_mask<n>(node_t& node, const board_t mask)
{
	node._occupy_black |= mask;
	node._occupy_knight |= mask;
	node._score -= 300;
}

template <>
void node_t::creator::set_mask<P>(node_t& node, const board_t mask)
{
	node._occupy_white |= mask;
	node._occupy_pawn |= mask;
	node._score += 100;
}

template <>
void node_t::creator::set_mask<p>(node_t& node, const board_t mask)
{
	node._occupy_black |= mask;
	node._occupy_pawn |= mask;
	node._score -= 100;
}

}
